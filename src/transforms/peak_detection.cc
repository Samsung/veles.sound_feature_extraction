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
#include <simd/wavelet.h>
#include "src/primitives/wavelet_filter_bank.h"
#include "src/make_unique.h"

namespace sound_feature_extraction {
namespace transforms {

PeakDetection::PeakDetection()
    : peaks_number_(kDefaultPeaksNumber),
      order_(kSortOrderBoth),
      type_(kExtremumTypeMaximum),
      min_pos_(0),
      max_pos_(1),
      swt_details_buffer_(nullptr, std::free),
      swt_type_(WAVELET_TYPE_DAUBECHIES),
      swt_order_(kDefaultWaveletOrder),
      swt_level_(0) {
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
  RegisterSetter("swt_type", [&](const std::string& value) {
    swt_type_ = Primitives::WaveletFilterBank::ParseWaveletType(value);
    return true;
  });
  RegisterSetter("swt_order", [&](const std::string& value) {
    int pv = Parse<int>("swt_order", value);
    if (pv < 0) {
      return false;
    }
    swt_order_ = pv;
    return true;
  });
  RegisterSetter("swt_level", [&](const std::string& value) {
    int pv = Parse<int>("swt_level", value);
    if (pv < 0) {
      return false;
    }
    swt_level_ = pv;
    return true;
  });
}

size_t PeakDetection::OnInputFormatChanged(size_t buffersCount) {
  outputFormat_->SetSize(peaks_number_);
  return buffersCount;
}

void PeakDetection::Initialize() const {
  if (swt_level_ != 0) {
    swt_details_buffer_ = std::uniquify(mallocf(inputFormat_->Size()),
                                        std::free);
    int count = MaxThreadsNumber();
    swt_buffers_.resize(count);
    for (int i = 0; i < count; i++) {
      swt_buffers_[i].data = std::uniquify(mallocf(inputFormat_->Size()),
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
                                   inputFormat_->Size(),
                                   swt_details_buffer_.get(),
                                   buf.data.get());
          for (int i = 2; i <= swt_level_; i++) {
            stationary_wavelet_apply(
                swt_type_, swt_order_, i, EXTENSION_TYPE_CONSTANT,
                buf.data.get(), inputFormat_->Size(), swt_details_buffer_.get(),
                buf.data.get());
          }
          detect_peaks(UseSimd(), buf.data.get(), inputFormat_->Size(), type_,
                       &results, &count);
          buf.mutex.unlock();
          executed = true;
          break;
        }
      }
    }
  } else {
    detect_peaks(UseSimd(), in, inputFormat_->Size(), type_, &results, &count);
  }
  if ((order_ & kSortOrderValue) != 0 && results != nullptr) {
    auto extr_type = type_;
    std::sort(results, results + count,
              [extr_type](const ExtremumPoint& f, const ExtremumPoint& s) {
                return (extr_type & kExtremumTypeMinimum) != 0?
                    f.value < s.value : f.value > s.value;
              });
  }
  int rcount = static_cast<int>(count) > peaks_number_? peaks_number_ : count;
  if (order_ == kSortOrderBoth  && results != nullptr) {
    std::sort(results, results + rcount,
              [](const ExtremumPoint& f, const ExtremumPoint& s) {
                return f.position < s.position;
              });
  }
  for (int i = 0; i < rcount; i++) {
    float pos = results[i].position;
    out[i][0] = min_pos_ + pos * (max_pos_ - min_pos_) / inputFormat_->Size();
    float val = results[i].value;
    if (swt_type_ == WAVELET_TYPE_DAUBECHIES) {
      for (int i = 0; i < swt_level_; i++) {
        val *= M_SQRT1_2;
      }
    }
    out[i][1] = val;
  }
  for (int i = count; i < peaks_number_; i++) {
    out[i][0] = min_pos_;
    out[i][1] = 0;
  }
  free(results);
}

REGISTER_TRANSFORM(PeakDetection);

}  // namespace transforms
}  // namespace sound_feature_extraction
