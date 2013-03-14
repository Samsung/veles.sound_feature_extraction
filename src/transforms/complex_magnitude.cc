/*! @file complex_magnitude.cc
 *  @brief Calculate the magnitude of each complex number.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/complex_magnitude.h"
#include <math.h>
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

namespace SpeechFeatureExtraction {
namespace Transforms {

ComplexMagnitude::ComplexMagnitude()
: UniformFormatTransform(SupportedParameters()) {
}

void ComplexMagnitude::OnInputFormatChanged() {
  outputFormat_->SetDuration(inputFormat_->Duration());
  outputFormat_->SetSamplingRate(inputFormat_->SamplingRate());
  if (inputFormat_->Size() % 2 == 1) {
    fprintf(stderr, "Input buffer size is odd (%zu), truncated\n",
            inputFormat_->Size());
  }
  outputFormat_->SetSize(inputFormat_->Size() / 2);
}

void ComplexMagnitude::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size() / 2);
}

void ComplexMagnitude::TypeSafeDo(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    auto input = in[i]->Data.get();
    auto output = (*out)[i]->Data.get();
    int length = inputFormat_->Size();
#ifdef __AVX__
    for (int i = 0; i < length - 15; i += 16) {
      __m256 vec1 = _mm256_load_ps(input + i);
      __m256 vec2 = _mm256_load_ps(input + i + 8);
      vec1 = _mm256_mul_ps(vec1, vec1);
      vec2 = _mm256_mul_ps(vec2, vec2);
      __m256 r1 = _mm256_permute2f128_ps(vec1, vec2, 0x20);
      __m256 r2 = _mm256_permute2f128_ps(vec1, vec2, 0x31);
      __m256 res = _mm256_hadd_ps(r1, r2);
      res = _mm256_sqrt_ps(res);
      _mm256_store_ps(output + i / 2, res);
    }
    for (int i = ((length >> 4) << 4); i < length; i += 2) {
      float re = input[i];
      float im = input[i + 1];
      output[i / 2] = sqrtf(re * re + im * im);
    }
#elif defined(__ARM_NEON__)
    for (int i = 0; i < length - 3; i += 4) {
      float32x4_t cvec = vld1q_f32(input + i);
      float32x4_t sqrvec = vmulq_f32(cvec, cvec);
      float32x2_t sums = vpadd_f32(vget_high_f32(sqrvec),
                                   vget_low_f32(sqrvec));
      vst1_f32(sums, output + i / 2);
    }
    for (int i = 0; i < length - 3; i++) {
      output[i] = sqrtf(output[i]);
    }
    for (int i = ((length >> 2) << 2); i < length; i += 2) {
      float re = input[i];
      float im = input[i + 1];
      output[i / 2] = sqrtf(re * re + im * im);
    }
#else
    for (int i = 0; i < length; i += 2) {
      float re = input[i];
      float im = input[i + 1];
      output[i / 2] = sqrtf(re * re + im * im);
    }
#endif
  }
}

REGISTER_TRANSFORM(ComplexMagnitude);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
