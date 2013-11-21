/*! @file preemphasis.cc
 *  @brief Number of time domain zero crossings of the signal.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/preemphasis.h"
#include <simd/instruction_set.h>

namespace sound_feature_extraction {
namespace transforms {

Preemphasis::Preemphasis()
    : value_(kDefaultValue) {
}

bool Preemphasis::validate_value(const float& value) noexcept {
  return value > 0 && value <= 1;
}

void Preemphasis::Do(const float* in,
                     float* out) const noexcept {

  Do(use_simd(), in, input_format_->Size(), value_, out);
}

void Preemphasis::Do(bool simd, const float* input, size_t length,
                     float k, float* output)
    noexcept {
  int ilength = length;
  output[0] = input[0];
  if (simd) {
#ifdef __AVX__
    const __m256 veck = _mm256_set1_ps(-k);
    for (int i = 1; i < ilength - 8; i += 8) {
      __m256 vecpre = _mm256_load_ps(input + i - 1);
      __m256 vec = _mm256_loadu_ps(input + i);
      vecpre = _mm256_mul_ps(vecpre, veck);
      vec = _mm256_add_ps(vec, vecpre);
      _mm256_storeu_ps(output + i, vec);
    }
    for (int i = ((ilength - 1) & ~7); i < ilength; i++) {
      output[i] = input[i] - k * input[i - 1];
    }
    return;
  } else {
#elif defined(__ARM_NEON__)
    const float32x4_t veck = vdupq_n_f32(-k);
    for (int i = 1; i < ilength - 4; i += 4) {
      float32x4_t vecpre = vld1q_f32(input + i - 1);
      float32x4_t vec = vld1q_f32(input + i);
      vec = vmlaq_f32(vec, vecpre, veck);
      vst1q_f32(output + i, vec);
    }
    for (int i = ((ilength - 1) & ~3); i < ilength; i++) {
      output[i] = input[i] - k * input[i - 1];
    }
    return;
  } else {
#else
  } {
#endif
    for (int i = 1; i < ilength; i++) {
      output[i] = input[i] - k * input[i - 1];
    }
  }
}

RTP(Preemphasis, value)
REGISTER_TRANSFORM(Preemphasis);

}  // namespace transforms
}  // namespace sound_feature_extraction
