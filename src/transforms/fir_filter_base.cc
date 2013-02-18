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
#include "src/primitives/convolute.h"
#include "src/primitives/arithmetic.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

FirFilterBase::FirFilterBase(
    const std::unordered_map<std::string, ParameterTraits>&
    supportedParameters) noexcept
: TransformBase(supportedParameters)
, length_(DEFAULT_FILTER_LENGTH) {
}

void FirFilterBase::Initialize() const noexcept {
  filter_.resize(length_);
  for (int i = 0; i < length_; i++) {
    filter_[i] = WindowFunction(i);
  }
  CalculateFilter(&filter_[0]);
  dataBuffer_.resize(inputFormat_.Size());
}

void FirFilterBase::OnInputFormatChanged() {
  outputFormat_ = inputFormat_;
}

void FirFilterBase::SetParameter(const std::string& name,
                                 const std::string& value) {
  if (name == "length") {
    int pv = Parse<int>(name, value);
    if (pv < MIN_FILTER_LENGTH || pv > MAX_FILTER_LENGTH) {
      throw new InvalidParameterValueException(name, value, Name());
    }
    length_ = pv;
  } else {
    SetFilterParameter(name, value);
  }
}

void FirFilterBase::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::Raw>& in,
    BuffersBase<Formats::Raw>* buffers) const noexcept {
  buffers->Initialize(in.Size(), outputFormat_.Size());
}

void FirFilterBase::TypeSafeDo(const BuffersBase<Formats::Raw>& in,
                               BuffersBase<Formats::Raw> *out)
const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    int16_to_float(in[i]->Data.get(), inputFormat_.Size(), &dataBuffer_[0]);
    convolute(&dataBuffer_[0], inputFormat_.Size(),
              &filter_[0], filter_.size(),
              &dataBuffer_[0]);
    float_to_int16(&dataBuffer_[0], inputFormat_.Size(), (*out)[i]->Data.get());
  }
}

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
