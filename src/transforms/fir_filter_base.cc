/*! @file convolution_filter_base.cc
 *  @brief Base class for all FIR filters.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/fir_filter_base.h"
#include <simd/arithmetic-inl.h>
#include <simd/convolve.h>
#include <mutex>

namespace SoundFeatureExtraction {
namespace Transforms {

FirFilterBase::FirFilterBase() noexcept
: length_(DEFAULT_FILTER_LENGTH),
  windowType_(WINDOW_TYPE_HAMMING) {
  RegisterSetter("length", [&](const std::string& value) {
    int pv = Parse<int>("length", value);
    if (pv < MIN_FILTER_LENGTH || pv > MAX_FILTER_LENGTH) {
      return false;
    }
    length_ = pv;
    return true;
  });
  RegisterSetter("window", [&](const std::string& value) {
    auto wti = kWindowTypeMap.find(value);
    if (wti == kWindowTypeMap.end()) {
      return false;
    }
    windowType_ = wti->second;
    return true;
  });
}

void FirFilterBase::Initialize() const noexcept {
  filter_.resize(length_);
  for (int i = 0; i < length_; i++) {
    filter_[i] = WindowElement(windowType_, length_, i);
  }
  CalculateFilter(&filter_[0]);

  convolutionHandles_.resize(MaxThreadsNumber());
  for (int i = 0; i < MaxThreadsNumber(); i++) {
    convolutionHandles_[i].handle = std::shared_ptr<ConvolutionHandle>(
        new ConvolutionHandle(convolve_initialize(inputFormat_->Size(),
                                                  filter_.size())),
        [](ConvolutionHandle* ptr) {
          convolve_finalize(*ptr);
          delete ptr;
        }
    );
    convolutionHandles_[i].mutex = std::make_shared<std::mutex>();
  }
}

size_t FirFilterBase::OnFormatChanged(size_t buffersCount) {
  outputFormat_->SetSize(inputFormat_->Size() + length_ - 1);
  return buffersCount;
}

void FirFilterBase::Do(const float* in,
                       float* out)
const noexcept {
  for (auto hp : convolutionHandles_) {
    if (hp.mutex->try_lock()) {
      convolve(*hp.handle, in, &filter_[0], out);
      hp.mutex->unlock();
      break;
    }
  }
}

}  // namespace Formats
}  // namespace SoundFeatureExtraction
