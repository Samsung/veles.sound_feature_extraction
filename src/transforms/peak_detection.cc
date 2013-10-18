/*! @file peak_detection.cc
 *  @brief Peak information extraction.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/peak_detection.h"
#include <algorithm>

namespace SoundFeatureExtraction {
namespace Transforms {

PeakDetection::PeakDetection()
    : peaks_number_(kDefaultPeaksNumber),
      order_(kSortOrderValue),
      type_(kExtremumTypeMaximum),
      min_pos_(0),
      max_pos_(1) {
  RegisterSetter("peaks", [&](const std::string& value) {
    int pv = Parse<int>("peaks", value);
    if (pv < 1) {
      return false;
    }
    peaks_number_ = pv;
    return true;
  });
  RegisterSetter("sort", [&](const std::string& value) {
    if (value == "value") {
      order_ = kSortOrderValue;
    } else if (value == "position") {
      order_ = kSortOrderPosition;
    } else {
      return false;
    }
    return true;
  });
   RegisterSetter("type", [&](const std::string& value) {
    if (value == "max") {
      type_ = kExtremumTypeMaximum;
    } else if (value == "min") {
      type_ = kExtremumTypeMinimum;
    } else if (value == "all") {
      type_ = kExtremumTypeBoth;
    } else {
      return false;
    }
    return true;
  });
  RegisterSetter("min_pos", [&](const std::string& value) {
    min_pos_ = Parse<float>("min_pos", value);
    return true;
  });
  RegisterSetter("max_pos", [&](const std::string& value) {
    max_pos_ = Parse<float>("max_pos", value);
    return true;
  });
}

size_t PeakDetection::OnInputFormatChanged(size_t buffersCount) {
  outputFormat_->SetSize(peaks_number_);
  return buffersCount;
}

void PeakDetection::Do(const float* in,
                       Formats::FixedArray<2> *out) const noexcept {
  ExtremumPoint* results;
  size_t count;
  detect_peaks(UseSimd(), in, inputFormat_->Size(), type_, &results, &count);
  if (order_ == kSortOrderValue && results != nullptr) {
    std::sort(results, results + count,
              [](const ExtremumPoint& f, const ExtremumPoint& s) {
                return f.value < s.value;
              });
  }
  for (int i = 0; i < static_cast<int>(count) && i < peaks_number_; i++) {
    out[i][0] = min_pos_ + results[i].position + 0.f / inputFormat_->Size() *
        (max_pos_ - min_pos_);
    out[i][1] = results[i].value;
  }
  for (int i = count; i < peaks_number_; i++) {
    out[i][0] = min_pos_;
    out[i][1] = 0;
  }
}

REGISTER_TRANSFORM(PeakDetection);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
