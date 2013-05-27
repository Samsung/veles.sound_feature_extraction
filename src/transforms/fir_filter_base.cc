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

namespace SoundFeatureExtraction {
namespace Transforms {

FirFilterBase::FirFilterBase() noexcept
: length_(DEFAULT_FILTER_LENGTH),
  windowType_(WINDOW_TYPE_HAMMING),
  convoluteHandle_(convolute_initialize(0, 0)) {
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

FirFilterBase::~FirFilterBase() {
  convolute_finalize(convoluteHandle_);
}

void FirFilterBase::Initialize() const noexcept {
  filter_.resize(length_);
  for (int i = 0; i < length_; i++) {
    filter_[i] = WindowElement(windowType_, length_, i);
  }
  CalculateFilter(&filter_[0]);
  dataBuffer_.resize(inputFormat_->Size());
  convolute_finalize(convoluteHandle_);
  convoluteHandle_ = convolute_initialize(inputFormat_->Size(), filter_.size());
}

void FirFilterBase::InitializeBuffers(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Raw16>* buffers) const noexcept {
  buffers->Initialize(in.Size(), outputFormat_->Size(),
                      in[0].AlignmentOffset());
}

void FirFilterBase::Do(const Formats::Raw16& in,
                       Formats::Raw16 *out)
const noexcept {
  int16_to_float(in.Data.get(), inputFormat_->Size(), &dataBuffer_[0]);
  convolute(convoluteHandle_, &dataBuffer_[0],
            &filter_[0], &dataBuffer_[0]);
  float_to_int16(&dataBuffer_[0], inputFormat_->Size(),
                 out->Data.get());
}

}  // namespace Formats
}  // namespace SoundFeatureExtraction
