/*! @file rolloff.cc
 *  @brief Measure of spectral shape.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/rolloff.h"
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include <simd/arithmetic-inl.h>

namespace sound_feature_extraction {
namespace transforms {

const float Rolloff::kDefaultRatio = 0.85f;

Rolloff::Rolloff() : ratio_(kDefaultRatio) {
  RegisterSetter("ratio", [&](const std::string& value) {
    float fv = Parse<float>("ratio", value);
    if (fv <= 0.f || fv >= 1.f) {
      return false;
    }
    ratio_ = fv;
    return true;
  });
}

void Rolloff::Do(const float* in,
                 float* out) const noexcept {
  *out = Do(UseSimd(), in, input_format_->Size(), ratio_) /
      input_format_->Duration();
}

int Rolloff::Do(bool simd, const float* input, size_t length,
                float ratio) noexcept {
  int ilength = length;
  float threshold;
  if (simd) {
#if defined(__AVX__) || defined(__ARM_NEON__)
    threshold = sum_elements(input, length) * ratio;
#ifdef __AVX__
    int j;
    float psum = 0.f;
    int max_index = ilength - 15;
    for (j = 0; j < max_index && psum < threshold; j += 16) {
      __m256 vec1 = _mm256_load_ps(input + j);
      __m256 vec2 = _mm256_load_ps(input + j + 8);
      __m256 psumvec = _mm256_hadd_ps(vec1, vec2);
      psumvec = _mm256_hadd_ps(psumvec, psumvec);
      psumvec = _mm256_hadd_ps(psumvec, psumvec);
      psum += ElementAt(psumvec, 0) + ElementAt(psumvec, 4);
    }
    if (j < max_index) {
      int i;
      for (i = j - 1; i > j - 16 && psum > threshold; i--) {
        psum -= input[i];
      }
      return i + 1;
    }
    int i;
    for (i = j - 16; i < ilength && psum < threshold; i++) {
      psum += input[i];
    }
    return i - 1;
#else
    int j;
    float psum = 0.f;
    int max_index = ilength - 7;
    for (j = 0; j < max_index && psum < threshold; j += 8) {
      float32x4_t vec1 = vld1q_f32(input + j);
      float32x4_t vec2 = vld1q_f32(input + j + 8);
      float32x4_t psumvec = vaddq_f32(vec1, vec2);
      float32x2_t psumvec2 = vpadd_f32(vget_high_f32(psumvec),
                                       vget_low_f32(psumvec));
      psum += vget_lane_f32(psumvec2, 0) + vget_lane_f32(psumvec2, 1);
    }
    if (j < max_index) {
      int i;
      for (i = j - 1; i > j - 16 && psum > threshold; i--) {
        psum -= input[i];
      }
      return i + 1;
    }
    int i;
    for (i = j - 16; i < ilength && psum < threshold; i++) {
      psum += input[i];
    }
    return i - 1;
#endif
  } else {
#else
  } {
#endif
    threshold = sum_elements_na(input, length) * ratio;
    float psum = 0.f;
    int i;
    for (i = 0; i < ilength && psum < threshold; i++) {
      psum += input[i];
    }
    return i - 1;
  }
}

REGISTER_TRANSFORM(Rolloff);

}  // namespace transforms
}  // namespace sound_feature_extraction
