/*! @file real_to_complex.cc
 *  @brief Converts real numbers to the corresponding complex numbers.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#define __STDC_LIMIT_MACROS
#include "src/transforms/real_to_complex.h"
#ifdef __AVX__
#include <immintrin.h>  // NOLINT(build/include_order)
#elif defined(__ARM_NEON__)
#include <arm_neon.h>  // NOLINT(build/include_order)
#endif

namespace SoundFeatureExtraction {
namespace Transforms {

size_t RealToComplex::OnFormatChanged(size_t buffersCount) {
  outputFormat_->SetSize(inputFormat_->Size() * 2);
  return buffersCount;
}

void RealToComplex::Do(const float* in,
                       float* out) const noexcept {
  Do(UseSimd(), in, inputFormat_->Size(), out);
}

void RealToComplex::Do(bool simd, const float* input, int length,
                       float* output) noexcept {
  if (simd) {
#ifdef __AVX__
#ifndef __AVX2__
    for (int i = 0; i < length - 7; i += 8) {
      __m256 vec = _mm256_load_ps(input + i);
      __m256 low = _mm256_unpacklo_ps(vec, _mm256_setzero_ps());
      __m256 high = _mm256_unpackhi_ps(vec, _mm256_setzero_ps());
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
      __m256 reslow = _mm256_permute2f128_ps(low, high, 32);
      __m256 reshigh = _mm256_permute2f128_ps(low, high, 49);
#pragma GCC diagnostic pop
      _mm256_store_ps(output + 2 * i, reslow);
      _mm256_store_ps(output + 2 * i + 8, reshigh);
    }
    for (int i = ((length >> 3) << 3); i < length; i++) {
      output[i * 2] = input[i];
      output[i * 2 + 1] = 0.f;
    }
#else
#error TODO: AVX2 introduces a full 256-bit permute which must be executed before _mm256_unpack*  // NOLINT(*)
#endif
  } else {
#elif defined(__ARM_NEON__)
    const float32x4_t zeros = { 0.f, 0.f, 0.f, 0.f };
    for (int i = 0; i < length - 3; i += 4) {
      float32x4_t vec = vld1q_f32(input + i);
      float32x4x2_t res = vzipq_f32(vec, zeros);
      vst2q_f32(output + i * 2, res);
    }

    for (int i = ((length >> 2) << 2); i < length; i++) {
      output[i * 2] = input[i];
      output[i * 2 + 1] = 0.f;
    }
  } else {
#else
  } {
#endif
    for (int i = 0; i < length; i++) {
      output[i * 2] = input[i];
      output[i * 2 + 1] = 0.f;
    }
  }
}

REGISTER_TRANSFORM(RealToComplex);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
