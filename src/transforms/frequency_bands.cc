/*! @file frequency_bands.cc
 *  @brief Cut the frequency bands from a signal.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 *
 *  @section License
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */

#include "src/transforms/frequency_bands.h"
#include <algorithm>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <boost/regex.hpp>
#pragma GCC diagnostic pop
#include "src/safe_omp.h"
#include "src/transforms/lowpass_filter.h"
#include "src/transforms/bandpass_filter.h"
#include "src/transforms/highpass_filter.h"
#include "src/stoi_function.h"

namespace sound_feature_extraction {
namespace transforms {

FilterOrders Parse(const std::string& value, identity<FilterOrders>) {
  if (value == "auto") {
    return FilterOrders();
  }
  FilterOrders result;
  std::transform(boost::sregex_token_iterator(value.begin(), value.end(),
                                              boost::regex("\\s*(\\d+)\\s*"),
                                              1),
                 boost::sregex_token_iterator(),
                 std::back_inserter(result),
                 std::stoi_function);
  if (result.empty()) {
    throw InvalidParameterValueException();
  }
  return result;
}

constexpr IIRFilterType FrequencyBands::kDefaultFilterType;

FrequencyBands::FrequencyBands()
    : number_(kDefaultBandsNumber),
      bands_(),
      filter_(kDefaultFilterType),
      lengths_() {
}

bool FrequencyBands::validate_number(const int& value) noexcept {
  return value >= 1 && value <= 8000;
}

bool FrequencyBands::validate_bands(const std::string& value) noexcept {
  if (value.empty()) {
    return true;
  }
  static const boost::regex all_regex("^\\s*(\\d+\\s*(\\s|$))+");
  boost::smatch match;
  if (!boost::regex_match(value, match, all_regex)) {
    return false;
  }
  std::vector<int> bands;
  std::transform(boost::sregex_token_iterator(value.begin(), value.end(),
                                              boost::regex("\\s*(\\d+)\\s*"),
                                              1),
                 boost::sregex_token_iterator(),
                 std::back_inserter(bands),
                 std::stoi_function);
  if (bands.size() < 1) {
    return false;
  }
  for (size_t i = 1; i < bands.size(); i++) {
    if (bands[i] <= bands[i - 1]) {
      return false;
    }
  }
  return true;
}
ALWAYS_VALID_TP(FrequencyBands, filter)

bool FrequencyBands::validate_lengths(const FilterOrders& value) noexcept {
  for (int len : value) {
    if (len < 1) {
      return false;
    }
  }
  return true;
}

const std::vector<std::shared_ptr<IIRFilterBase>>&
FrequencyBands::filters() const {
  return filters_;
}

void FrequencyBands::Initialize() const {
  filters_.clear();
  std::string bands = bands_;
  if (bands.empty()) {
    for (int i = 1; i < number_; i++) {
      bands += std::to_string(static_cast<int>(
          input_format_->SamplingRate() / (2.f * number_) * i)) + " ";
    }
  }

  int last_freq = 0, index = 0;
  static const boost::regex numbers_regex("\\s*(\\d+)\\s*");
  static const boost::sregex_token_iterator empty;
  boost::sregex_token_iterator bands_iterator(
        bands.begin(), bands.end(), numbers_regex, 1);
  assert(bands_iterator != empty);
  while (bands_iterator != empty) {
    int freq = std::stoi(*bands_iterator++);
    if (freq > input_format_->SamplingRate() / 2) {
      WRN("Warning: the bands after %i (defined by sampling "
          "rate %i) will be discarded (first greater band was "
          "%i).\n",
          input_format_->SamplingRate() / 2,
          input_format_->SamplingRate(),
          freq);
      break;
    }
    std::shared_ptr<IIRFilterBase> filter;
    if (last_freq == 0) {
      auto f = std::make_shared<LowpassFilter>();
      f->set_frequency(freq);
      filter = f;
    } else {
      auto f = std::make_shared<BandpassFilter>();
      f->set_frequency_low(last_freq);
      f->set_frequency_high(freq);
      filter = f;
    }
    SetupFilter(index++, freq, filter.get());
    filters_.push_back(filter);
    last_freq = freq;
  }
  // Append high-pass filter
  auto filter = std::make_shared<HighpassFilter>();
  filter->set_frequency(last_freq);
  SetupFilter(index, last_freq, filter.get());
  filters_.push_back(filter);

  for (const auto& filter : filters_) {  // NOLINT(*)
    filter->SetInputFormat(input_format_, 1);
    filter->Initialize();
  }
}

void FrequencyBands::SetupFilter(size_t index, int frequency,
                                 IIRFilterBase* filter) const {
  filter->set_type(filter_);
  auto ratio = frequency / input_format_->SamplingRate() * 2;
  if (lengths_.size() > index) {
    filter->set_length(lengths_[index]);
  } else {
    if (ratio < 0.1f) {
      filter->set_length(64);
    } else if (ratio < 0.3f) {
      filter->set_length(128);
    }
  }
}

void FrequencyBands::Do(const BuffersBase<float*>& in,
                        BuffersBase<float*>* out) const noexcept {
#ifdef HAVE_OPENMP
  #pragma omp parallel for num_threads(threads_number())
#endif
  for (size_t i = 0; i < in.Count(); i++) {
    filters_[i % filters_.size()]->Do(in[i], (*out)[i]);
  }
}

RTP(FrequencyBands, number)
RTP(FrequencyBands, bands)
RTP(FrequencyBands, filter)
RTP(FrequencyBands, lengths)
REGISTER_TRANSFORM(FrequencyBands);

}  // namespace transforms
}  // namespace sound_feature_extraction
