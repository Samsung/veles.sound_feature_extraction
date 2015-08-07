/*! @file peak_detection.cc
 *  @brief Peak information extraction.
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

#include "src/transforms/peak_detection.h"
#include <algorithm>
#include <simd/wavelet.h>
#include "src/make_unique.h"

namespace sound_feature_extraction {

SortOrder Parse(const std::string& value, identity<SortOrder>) {
  static const std::unordered_map<std::string, SortOrder> map {
    { internal::kSortOrderValueStr, kSortOrderValue },
    { internal::kSortOrderPositionStr, kSortOrderPosition },
    { internal::kSortOrderBothStr, kSortOrderBoth }
  };
  auto soit = map.find(value);
  if (soit == map.end()) {
    throw InvalidParameterValueException();
  }
  return soit->second;
}

ExtremumType Parse(const std::string& value, identity<ExtremumType>) {
  static const std::unordered_map<std::string, ExtremumType> map {
    { internal::kExtremumTypeMinimumStr, kExtremumTypeMinimum },
    { internal::kExtremumTypeMaximumStr, kExtremumTypeMaximum },
    { internal::kExtremumTypeBothStr, kExtremumTypeBoth }
  };
  auto soit = map.find(value);
  if (soit == map.end()) {
    throw InvalidParameterValueException();
  }
  return soit->second;
}

namespace transforms {

PeakDetection::PeakDetection()
    : sort_(kDefaultSortOrder),
      number_(kDefaultPeaksNumber),
      type_(kDefaultExtremumType),
      min_pos_(kDefaultMinPos),
      max_pos_(kDefaultMaxPos),
      swt_type_(kDefaultSWTType),
      swt_order_(kDefaultWaveletOrder),
      swt_level_(kDefaultSWTLevel),
      swt_details_buffer_(nullptr, std::free) {
}
ALWAYS_VALID_TP(PeakDetection, sort)

bool PeakDetection::validate_number(const int& value) noexcept {
  return value >= 1;
}

ALWAYS_VALID_TP(PeakDetection, type)
ALWAYS_VALID_TP(PeakDetection, min_pos)
ALWAYS_VALID_TP(PeakDetection, max_pos)
ALWAYS_VALID_TP(PeakDetection, swt_type)

bool PeakDetection::validate_swt_order(const int& value) noexcept {
  return value >= 2;
}

bool PeakDetection::validate_swt_level(const int& value) noexcept {
  return value >= 0;
}

size_t PeakDetection::OnInputFormatChanged(size_t buffersCount) {
  output_format_->SetSize(number_);
  return buffersCount;
}

void PeakDetection::Initialize() const {
  if (swt_level_ != 0) {
    swt_details_buffer_ = std::uniquify(mallocf(input_format_->Size()),
                                        std::free);
    int count = threads_number();
    swt_buffers_.resize(count);
    for (int i = 0; i < count; i++) {
      swt_buffers_[i].data = std::uniquify(mallocf(input_format_->Size()),
                                           std::free);
    }
  }
}

void PeakDetection::Do(const float* in,
                       formats::FixedArray<2>* out) const noexcept {
  ExtremumPoint* results;
  size_t count;
  if (swt_level_ > 0) {
    bool executed = false;
    while (!executed) {
      for (auto& buf : swt_buffers_) {
        if (buf.mutex.try_lock()) {
          // swt_details_buffer_ is shared among all the threads; we just
          // do not care because it is not really used.
          stationary_wavelet_apply(swt_type_, swt_order_, 1,
                                   EXTENSION_TYPE_CONSTANT, in,
                                   input_format_->Size(),
                                   swt_details_buffer_.get(),
                                   buf.data.get());
          for (int i = 2; i <= swt_level_; i++) {
            stationary_wavelet_apply(
                swt_type_, swt_order_, i, EXTENSION_TYPE_CONSTANT,
                buf.data.get(), input_format_->Size(),
                swt_details_buffer_.get(), buf.data.get());
          }
          detect_peaks(use_simd(), buf.data.get(), input_format_->Size(), type_,
                       &results, &count);
          buf.mutex.unlock();
          executed = true;
          break;
        }
      }
    }
  } else {
    detect_peaks(use_simd(), in, input_format_->Size(), type_, &results,
                 &count);
  }
  if ((sort_ & kSortOrderValue) != 0 && results != nullptr) {
    auto extr_type = type_;
    std::sort(results, results + count,
              [extr_type](const ExtremumPoint& f, const ExtremumPoint& s) {
                return (extr_type & kExtremumTypeMinimum) != 0?
                    f.value < s.value : f.value > s.value;
              });
  }
  int rcount = static_cast<int>(count) > number_? number_ : count;
  if (sort_ == kSortOrderBoth  && results != nullptr) {
    std::sort(results, results + rcount,
              [](const ExtremumPoint& f, const ExtremumPoint& s) {
                return f.position < s.position;
              });
  }
  for (int i = 0; i < rcount; i++) {
    float pos = results[i].position;
    out[i][0] = min_pos_ + pos * (max_pos_ - min_pos_) / input_format_->Size();
    float val = results[i].value;
    if (swt_type_ == WAVELET_TYPE_DAUBECHIES) {
      for (int i = 0; i < swt_level_; i++) {
        val *= M_SQRT1_2;
      }
    }
    out[i][1] = val;
  }
  for (int i = count; i < number_; i++) {
    out[i][0] = min_pos_;
    out[i][1] = 0;
  }
  free(results);
}

RTP(PeakDetection, sort)
RTP(PeakDetection, number)
RTP(PeakDetection, type)
RTP(PeakDetection, min_pos)
RTP(PeakDetection, max_pos)
RTP(PeakDetection, swt_type)
RTP(PeakDetection, swt_order)
RTP(PeakDetection, swt_level)
REGISTER_TRANSFORM(PeakDetection);

}  // namespace transforms
}  // namespace sound_feature_extraction
