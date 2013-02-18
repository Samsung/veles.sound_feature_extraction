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

namespace SpeechFeatureExtraction {
namespace Transforms {

DFT::DFT()
: TransformBase(SupportedParameters()) {
}

void DFT::Initialize() const noexcept {
  int length = outputFormat_.Duration() * outputFormat_.SamplingRate() / 1000;
  auto fftPlan = fftf_init_batch(
      FFTF_TYPE_REAL,
      IsInverse()? FFTF_DIRECTION_BACKWARD : FFTF_DIRECTION_FORWARD,
      FFTF_DIMENSION_1D, &length, FFTF_NO_OPTIONS, 100,
      const float *const *inputs,
      float *const *outputs);
  fftPlan_ = std::shared_ptr(fftPlan, [](FFTFInstance* inst) {
    fftf_destroy(inst);
  });
}

void DFT::OnInputFormatChanged() {
}

void DFT::SetParameter(const std::string& name,
                       const std::string& value) {
}

void DFT::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::Window16>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size());
}

void DFT::TypeSafeDo(
    const BuffersBase<Formats::Window16>& in,
    BuffersBase<Formats::WindowF> *out) const noexcept {
  for (int i = 0; i < in.Size(); i++) {

  }
}

REGISTER_TRANSFORM(DFT);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
