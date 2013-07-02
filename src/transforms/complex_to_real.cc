/*! @file complex_to_real.cc
 *  @brief Converts complex numbers to the corresponding real numbers.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#define __STDC_LIMIT_MACROS
#include "src/transforms/complex_to_real.h"
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

namespace SoundFeatureExtraction {
namespace Transforms {

size_t ComplexToReal::OnFormatChanged(size_t buffersCount) {
  outputFormat_->SetSize(inputFormat_->Size() / 2);
  return buffersCount;
}

void ComplexToReal::Do(const float* in,
    float* out) const noexcept {
  Do(UseSimd(), in, inputFormat_->Size(), out);
}

void ComplexToReal::Do(bool simd, const float* input, int length,
                       float* output) noexcept {
  if (simd) {
#ifdef __AVX__
    for (int i = 0; i < length - 15; i += 16) {
      __m256 vec1 = _mm256_load_ps(input + i);
      __m256 vec2 = _mm256_load_ps(input + i + 8);
      __m256 vec2even = _mm256_shuffle_ps(vec2, vec2, 160);
      __m256 low = _mm256_permute2f128_ps(vec1, vec2even, 32);
      __m256 high = _mm256_permute2f128_ps(vec1, vec2even, 49);
      __m256 result = _mm256_shuffle_ps(low, high, 136);
      _mm256_store_ps(output + i / 2, result);
    }
    for (int i = ((length >> 4) << 4); i < length; i += 2) {
      output[i / 2] = input[i];
    }
  } else {
#elif defined(__ARM_NEON__)
    for (int i = 0; i < length - 7; i += 8) {
      float32x4_t vec1 = vld1q_f32(input + i);
      float32x4_t vec2 = vld1q_f32(input + i + 4);
      float32x4x2_t result = vuzpq_f32(vec2, vec1);
      vst1q_f32(output + i / 2, result.val[0]);
    }
    for (int i = ((length >> 3) << 3); i < length; i += 2) {
      output[i / 2] = input[i];
    }
  } else {
#else
  } {
#endif
    for (int i = 0; i < length; i += 2) {
      output[i / 2] = input[i];
    }
  }
}

REGISTER_TRANSFORM(ComplexToReal);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
