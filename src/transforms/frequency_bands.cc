/*! @file frequency_bands.cc
 *  @brief Cut the frequency bands from a signal.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/frequency_bands.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <boost/regex.hpp>
#pragma GCC diagnostic pop
#include "src/safe_omp.h"
#include "src/transforms/fork.h"
#include "src/transforms/lowpass_filter.h"
#include "src/transforms/bandpass_filter.h"
#include "src/transforms/highpass_filter.h"

namespace SoundFeatureExtraction {
namespace Transforms {

const int FrequencyBands::kDefaultBandsNumber = Fork::kDefaultFactor;
constexpr const char* FrequencyBands::kDefaultFilterType;

FrequencyBands::FrequencyBands()
    : bands_number_(kDefaultBandsNumber),
      lengths_config_("auto"),
      filter_type_(kDefaultFilterType) {
  RegisterSetter("number", [&](const std::string& value) {
    int pv = Parse<int>("number", value);
    if (pv < 1 || pv > 8000) {
      return false;
    }
    bands_number_ = pv;
    return true;
  });
  RegisterSetter("bands", [&](const std::string& value) {
    static const boost::regex allRegex("^\\s*(\\d+\\s*(\\s|$))+");
    boost::smatch match;
    if (!boost::regex_match(value, match, allRegex)) {
      return false;
    }
    static const boost::regex bandsRegex("\\s*(\\d+)\\s*");
    static const boost::sregex_token_iterator empty;
    boost::sregex_token_iterator bandsIterator(
          value.begin(), value.end(), bandsRegex, 1);
    assert(bandsIterator != empty);
    int oldFreq = 0;
    while (bandsIterator != empty) {
      int freq = std::stoi(*bandsIterator++);
      if (freq <= oldFreq) {
        return false;
      }
      oldFreq = freq;
    }
    bands_config_ = value;
    return true;
  });
  RegisterSetter("filter", [&](const std::string& value) {
    auto fti = IIRFilterBase::kFilterTypeMap.find(value);
    if (fti == IIRFilterBase::kFilterTypeMap.end()) {
      return false;
    }
    filter_type_ = value;
    return true;
  });
  RegisterSetter("lengths", [&](const std::string& value) {
    if (value == "auto") {
      return true;
    }
    static const boost::regex allRegex("^\\s*(\\d+\\s*(\\s|$))+");
    boost::smatch match;
    if (!boost::regex_match(value, match, allRegex)) {
      return false;
    }
    lengths_config_ = value;
    return true;
  });
}

int FrequencyBands::bands_number() const noexcept {
  return bands_number_;
}

void FrequencyBands::Initialize() const {
  filters_.clear();
  if (bands_config_ == "") {
    for (int i = 1; i < bands_number_; i++) {
      bands_config_ += std::to_string(static_cast<int>(
          inputFormat_->SamplingRate() / (2.f * bands_number_) * i)) + " ";
    }
  }

  int last_freq = 0, index = 0;
  static const boost::regex numbersRegex("\\s*(\\d+)\\s*");
  static const boost::sregex_token_iterator empty;
  if (lengths_config_ != "auto") {
    boost::sregex_token_iterator lengthsIterator(
          lengths_config_.begin(), lengths_config_.end(), numbersRegex, 1);
    assert(lengthsIterator != empty);
    while (lengthsIterator != empty) {
      lengths_.push_back(std::stoi(*lengthsIterator++));
    }
  }
  boost::sregex_token_iterator bandsIterator(
        bands_config_.begin(), bands_config_.end(), numbersRegex, 1);
  assert(bandsIterator != empty);
  while (bandsIterator != empty) {
    int freq = std::stoi(*bandsIterator++);
    if (freq > inputFormat_->SamplingRate() / 2) {
      WRN("Warning: the bands after %i (defined by sampling "
          "rate %i) will be discarded (first greater band was "
          "%i).\n",
          inputFormat_->SamplingRate() / 2,
          inputFormat_->SamplingRate(),
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
  bands_number_ = filters_.size();

  for (const auto& filter : filters_) {
    filter->SetInputFormat(inputFormat_, 1);
    filter->Initialize();
  }
}

void FrequencyBands::SetupFilter(size_t index, int frequency,
                                 IIRFilterBase* filter) const {
  filter->SetParameter("type", filter_type_);
  auto ratio = frequency / inputFormat_->SamplingRate() * 2;
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
  auto rem = in.Count() % filters_.size();
  if (rem != 0) {
    WRN("Warning: the number of windows (%zu) is not a multiple "
        "of bands number (%zu). The remainder is discarded.",
        in.Count(), filters_.size());
  }

#ifdef HAVE_OPENMP
   #pragma omp parallel for num_threads(this->MaxThreadsNumber())
#endif
  for (size_t i = 0; i < in.Count(); i += filters_.size()) {
    for (int j = 0; j < static_cast<int>(filters_.size()); j++) {
      filters_[j]->Do(in[i + j], (*out)[i + j]);
    }
  }
}

REGISTER_TRANSFORM(FrequencyBands);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
