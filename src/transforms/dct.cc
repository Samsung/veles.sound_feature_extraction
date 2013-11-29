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
#include <simd/arithmetic-inl.h>

namespace sound_feature_extraction {
namespace transforms {

size_t DCT::OnFormatChanged(size_t buffersCount) {
  fftf_ensure_is_supported(FFTF_TYPE_DCT, input_format_->Size());
  return buffersCount;
}

void DCT::Do(const BuffersBase<float*>& in,
             BuffersBase<float*>* out) const noexcept {
  int length = output_format_->Size();
  std::vector<const float*> inputs(in.Count());
  std::vector<float*> outputs(in.Count());
  for (size_t i = 0; i < in.Count(); i++) {
    inputs[i] = in[i];
    outputs[i] = (*out)[i];
  }
  auto fft_plan = std::unique_ptr<FFTFInstance, void (*)(FFTFInstance *)>(
      fftf_init_batch(
          FFTF_TYPE_DCT,
          FFTF_DIRECTION_FORWARD,
          FFTF_DIMENSION_1D,
          &length,
          FFTF_NO_OPTIONS,
          in.Count(),
          &inputs[0], &outputs[0]),
      fftf_destroy);

  fftf_calc(fft_plan.get());
}

size_t DCTInverse::OnFormatChanged(size_t buffersCount) {
  fftf_ensure_is_supported(FFTF_TYPE_DCT, input_format_->Size());
  return buffersCount;
}

void DCTInverse::Do(const BuffersBase<float*>& in,
                    BuffersBase<float*>* out) const noexcept {
  int length = output_format_->Size();
  std::vector<const float*> inputs(in.Count());
  std::vector<float*> outputs(in.Count());
  for (size_t i = 0; i < in.Count(); i++) {
    inputs[i] = in[i];
    outputs[i] = (*out)[i];
  }
  auto fft_plan = std::unique_ptr<FFTFInstance, void (*)(FFTFInstance *)>(
      fftf_init_batch(
          FFTF_TYPE_DCT,
          FFTF_DIRECTION_BACKWARD,
          FFTF_DIMENSION_1D,
          &length,
          FFTF_NO_OPTIONS,
          in.Count(),
          &inputs[0], &outputs[0]),
      fftf_destroy);

  fftf_calc(fft_plan.get());
  for (size_t i = 0; i < in.Count(); i++) {
    real_multiply_scalar(outputs[i], length, 0.5f / length, outputs[i]);
  }
}

REGISTER_TRANSFORM(DCT);
REGISTER_TRANSFORM(DCTInverse);

}  // namespace transforms
}  // namespace sound_feature_extraction
