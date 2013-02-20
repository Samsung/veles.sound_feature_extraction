/*! @file dft.cc
 *  @brief Discrete Fourier Transform using FFT.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/dft.h"
#include <fftf/api.h>
#include "src/primitives/arithmetic.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

DFT::DFT()
: UniformFormatTransform(SupportedParameters()) {
}

bool DFT::HasInverse() const noexcept {
  return true;
}

void DFT::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_.SamplesCount());
}

void DFT::TypeSafeDo(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF> *out) const noexcept {
  int length = outputFormat_.SamplesCount();
  std::vector<float*> inputs(in.Size()), outputs(in.Size());
  for (size_t i = 0; i < in.Size(); i++) {
    inputs[i] = in[i]->Data.get();
    outputs[i] = (*out)[i]->Data.get();
  }
  auto fftPlan = fftf_init_batch(
      FFTF_TYPE_REAL,
      IsInverse()? FFTF_DIRECTION_BACKWARD : FFTF_DIRECTION_FORWARD,
      FFTF_DIMENSION_1D,
      &length,
      FFTF_NO_OPTIONS,
      in.Size(),
      &inputs[0], &outputs[0]);

  fftf_calc(fftPlan);
  if (IsInverse()) {
    for (size_t i = 0; i < in.Size(); i++) {
      real_multiply_scalar(1.0f / length, length, outputs[i]);
    }
  }

  fftf_destroy(fftPlan);
}

REGISTER_TRANSFORM(DFT);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
