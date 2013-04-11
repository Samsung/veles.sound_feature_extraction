/*! @file dct.cc
 *  @brief Discrete Cosine Transform calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/dct.h"
#include <fftf/api.h>
#include <vector>
#include "src/primitives/arithmetic-inl.h"

namespace SoundFeatureExtraction {
namespace Transforms {

bool DCT::HasInverse() const noexcept {
  return true;
}

void DCT::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size());
}

void DCT::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
  int length = outputFormat_->Size();
  std::vector<float*> inputs(in.Size()), outputs(in.Size());
  for (size_t i = 0; i < in.Size(); i++) {
    inputs[i] = in[i]->Data.get();
    outputs[i] = (*out)[i]->Data.get();
  }
  auto fftPlan = std::unique_ptr<FFTFInstance, void (*)(FFTFInstance *)>(
      fftf_init_batch(
          FFTF_TYPE_DCT,
          IsInverse()? FFTF_DIRECTION_BACKWARD : FFTF_DIRECTION_FORWARD,
          FFTF_DIMENSION_1D,
          &length,
          FFTF_NO_OPTIONS,
          in.Size(),
          &inputs[0], &outputs[0]),
      fftf_destroy);

  fftf_calc(fftPlan.get());
  if (IsInverse()) {
    for (size_t i = 0; i < in.Size(); i++) {
      real_multiply_scalar(outputs[i], length, 0.5f / length, outputs[i]);
    }
  }
}

REGISTER_TRANSFORM(DCT);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
