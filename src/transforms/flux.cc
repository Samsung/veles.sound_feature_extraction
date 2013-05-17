/*! @file flux.cc
 *  @brief Measure of spectral change.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/flux.h"
#include <math.h>
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include <simd/arithmetic-inl.h>

namespace SoundFeatureExtraction {
namespace Transforms {

void Flux::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<float>* buffers) const noexcept {
  buffers->Initialize(in.Size());
}

void Flux::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<float> *out) const noexcept {
  *(*out)[0] = 0.f;
  for (size_t i = 1; i < in.Size(); i++) {
    *(*out)[i] = Do(true, in[i]->Data.get(), inputFormat_->Size(),
                    in[i - 1]->Data.get());
  }
}

float Flux::Do(bool simd, const float* input, size_t length,
               const float* prev) noexcept {
  int ilength = length;
  if (simd) {
#ifdef __AVX__
    __m256 diff = _mm256_setzero_ps();
    for (int i = 0; i < ilength - 15; i += 16) {
      __m256 vec11 = _mm256_load_ps(input + i);
      __m256 vec12 = _mm256_load_ps(input + i + 8);
      __m256 vec21 = _mm256_load_ps(prev + i);
      __m256 vec22 = _mm256_load_ps(prev + i + 8);
      __m256 diff1 = _mm256_sub_ps(vec11, vec21);
      __m256 diff2 = _mm256_sub_ps(vec12, vec22);
#ifndef __AVX2__
      diff1 = _mm256_mul_ps(diff1, diff1);
      diff2 = _mm256_mul_ps(diff2, diff2);
      diff = _mm256_add_ps(diff, diff1);
      diff = _mm256_add_ps(diff, diff2);
#else
      diff = _mm256_fmadd_ps(diff1, diff1, diff);
      diff = _mm256_fmadd_ps(diff2, diff2, diff);
#endif
    }
    diff = _mm256_hadd_ps(diff, diff);
    diff = _mm256_hadd_ps(diff, diff);
    float sqr = ElementAt(diff, 0) + ElementAt(diff, 4);
    for (int i = ((ilength >> 4) << 4); i < ilength; i++) {
      float val = input[i] - prev[i];
      sqr += val * val;
    }
    return sqrtf(sqr);
  } else {
#elif defined(__ARM_NEON__)
    float32x4_t diff = vdupq_n_f32(0.f);
    for (int i = 0; i < ilength - 7; i += 8) {
      float32x4_t vec11 = vld1q_f32(input + i);
      float32x4_t vec12 = vld1q_f32(input + i + 4);
      float32x4_t vec21 = vld1q_f32(prev + i);
      float32x4_t vec22 = vld1q_f32(prev + i + 4);
      float32x4_t diff1 = vsubq_f32(vec11, vec21);
      float32x4_t diff2 = vsubq_f32(vec12, vec22);
      diff = vmlaq_f32(diff1, diff1, diff);
      diff = vmlaq_f32(diff2, diff2, diff);
    }
    float32x2_t diff2 = vpadd_f32(vget_high_f32(diff),
                                  vget_low_f32(diff));
    float sqr = vget_lane_f32(diff2, 0) + vget_lane_f32(diff2, 1);
    for (int i = ((ilength >> 3) << 3); i < ilength; i++) {
      float val = input[i] - prev[i];
      sqr += val * val;
    }
    return sqrtf(sqr);
  } else {
#else
  } {
#endif
    float sqr = 0.f;
    for (int i = 0; i < ilength; i++) {
      float val = input[i] - prev[i];
      sqr += val * val;
    }
    return sqrtf(sqr);
  }
}

REGISTER_TRANSFORM(Flux);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
