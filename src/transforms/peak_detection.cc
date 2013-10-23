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
      order_(kSortOrderBoth),
      type_(kExtremumTypeMaximum),
      min_pos_(0),
      max_pos_(1) {
  RegisterSetter("number", [&](const std::string& value) {
    int pv = Parse<int>("number", value);
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
    } else if (value == "both") {
      order_ = kSortOrderBoth;
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
                       Formats::FixedArray<2>* out) const noexcept {
  ExtremumPoint* results;
  size_t count;
  detect_peaks(UseSimd(), in, inputFormat_->Size(), type_, &results, &count);
  if ((order_ & kSortOrderValue) != 0 && results != nullptr) {
    auto extr_type = type_;
    std::sort(results, results + count,
              [extr_type](const ExtremumPoint& f, const ExtremumPoint& s) {
                return (extr_type & kExtremumTypeMinimum) != 0?
                    f.value < s.value : f.value > s.value;
              });
  }
  int rcount = static_cast<int>(count) > peaks_number_? peaks_number_ : count;
  if ((order_ & kSortOrderPosition) != 0  && results != nullptr) {
    std::sort(results, results + rcount,
              [](const ExtremumPoint& f, const ExtremumPoint& s) {
                return f.position < s.position;
              });
  }
  for (int i = 0; i < rcount; i++) {
    out[i][0] = min_pos_ + results[i].position + 0.f / inputFormat_->Size() *
        (max_pos_ - min_pos_);
    out[i][1] = results[i].value;
  }
  for (int i = count; i < peaks_number_; i++) {
    out[i][0] = min_pos_;
    out[i][1] = 0;
  }
  free(results);
}

REGISTER_TRANSFORM(PeakDetection);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
