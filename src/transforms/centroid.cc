/*! @file centroid.cc
 *  @brief Centroid (window's center of mass in frequency domain) calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/centroid.h"
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include <simd/avx_extra.h>

namespace SoundFeatureExtraction {
namespace Transforms {

void Centroid::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<float>* buffers) const noexcept {
  buffers->Initialize(in.Size());
}

void Centroid::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<float> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    (*out)[i] = Do(true, in[i].Data.get(), inputFormat_->Size()) *
        inputFormat_->SamplingRate() / (2 * inputFormat_->SamplesCount());
  }
}

float Centroid::Do(bool simd, const float* input, size_t length)
    noexcept {
  int ilength = length;
  if (simd) {
#ifdef __AVX__
    __m256 upperSums = _mm256_setzero_ps(), lowerSums = _mm256_setzero_ps();
    __m256 indexes = { 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f };
    const __m256 step = _mm256_set1_ps(8.f);
    for (int i = 0; i < ilength - 15; i += 16) {
      __m256 vec1 = _mm256_load_ps(input + i);
      __m256 vec2 = _mm256_load_ps(input + i + 8);
      lowerSums = _mm256_add_ps(lowerSums, vec1);
      lowerSums = _mm256_add_ps(lowerSums, vec2);
      vec1 = _mm256_mul_ps(vec1, indexes);
      indexes = _mm256_add_ps(indexes, step);
      vec2 = _mm256_mul_ps(vec2, indexes);
      upperSums = _mm256_add_ps(upperSums, vec1);
      upperSums = _mm256_add_ps(upperSums, vec2);
      indexes = _mm256_add_ps(indexes, step);
    }
    __m256 sums = _mm256_hadd_ps(lowerSums, upperSums);
    sums = _mm256_hadd_ps(sums, sums);
    float lowerSum = ElementAt(sums, 0) + ElementAt(sums, 4),
        upperSum = ElementAt(sums, 1) + ElementAt(sums, 5);
    for (int i = ((ilength >> 4) << 4); i < ilength; i++) {
      float val = input[i];
      lowerSum += val;
      upperSum += i * val;
    }
    return upperSum / lowerSum;
  } else {
#elif defined(__ARM_NEON__)
    float32x4_t upperSums = vdupq_n_f32(0.f), lowerSums = vdupq_n_f32(0.f);
    float32x4_t indexes = { 0.f, 1.f, 2.f, 3.f };
    const float32x4_t step = vdupq_n_f32(4.f);
    for (int i = 0; i < ilength - 7; i += 8) {
      float32x4_t vec1 = vld1q_f32(input + i);
      float32x4_t vec2 = vld1q_f32(input + i + 4);
      lowerSums = vaddq_f32(lowerSums, vec1);
      lowerSums = vaddq_f32(lowerSums, vec2);
      vec1 = vmulq_f32(vec1, indexes);
      indexes = vaddq_f32(indexes, step);
      vec2 = vmulq_f32(vec2, indexes);
      upperSums = vaddq_f32(upperSums, vec1);
      upperSums = vaddq_f32(upperSums, vec2);
      indexes = vaddq_f32(indexes, step);
    }
    float32x2_t lowerSums2 = vpadd_f32(vget_high_f32(lowerSums),
                                       vget_low_f32(lowerSums));
    float32x2_t upperSums2 = vpadd_f32(vget_high_f32(upperSums),
                                       vget_low_f32(upperSums));
    float lowerSum =
            vget_lane_f32(lowerSums2, 0) + vget_lane_f32(lowerSums2, 1),
        upperSum =
            vget_lane_f32(upperSums2, 0) + vget_lane_f32(upperSums2, 1);
    for (int i = ((ilength >> 3) << 3); i < ilength; i++) {
      float val = input[i];
      lowerSum += val;
      upperSum += i * val;
    }
    return upperSum / lowerSum;
  } else {
#else
  } {
#endif
    float upperSum = 0, lowerSum = 0;
    for (int i = 0; i < ilength; i++) {
      float val = input[i];
      lowerSum += val;
      upperSum += i * val;
    }
    return upperSum / lowerSum;
  }
}

REGISTER_TRANSFORM(Centroid);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
