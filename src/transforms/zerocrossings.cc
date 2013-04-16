/*! @file zerocrossings.cc
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

#include "src/transforms/zerocrossings.h"
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include "src/primitives/avx_extra.h"

namespace SoundFeatureExtraction {
namespace Transforms {

void ZeroCrossings::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<int32_t>* buffers) const noexcept {
  buffers->Initialize(in.Size());
}

void ZeroCrossings::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<int32_t> *out) const noexcept {
  int length = inputFormat_->Size();
  for (size_t i = 0; i < in.Size(); i++) {
    *(*out)[i] = Do(true, in[i]->Data.get(), length);
  }
}

int ZeroCrossings::Do(bool simd, const float* input, size_t length)
    noexcept {
  int ilength = length;
  if (simd) {
#ifdef __AVX__
    __m256 crossings = _mm256_setzero_ps();
    const __m256 zeros = _mm256_setzero_ps();
    const __m256 ones = _mm256_set1_ps(1.f);
    for (int i = 0; i < ilength - 8; i += 8) {
      __m256 vecpre = _mm256_load_ps(input + i);
      __m256 zerocheck =  _mm256_cmp_ps(vecpre, zeros, _CMP_EQ_OQ);
      __m256 vec = _mm256_loadu_ps(input + i + 1);
      __m256 tmp = _mm256_mul_ps(vecpre, vec);
      tmp = _mm256_cmp_ps(tmp, zeros, _CMP_LT_OQ);
      tmp = _mm256_or_ps(tmp, zerocheck);
      tmp = _mm256_blendv_ps(zeros, ones, tmp);
      crossings = _mm256_add_ps(crossings, tmp);
    }
    crossings = _mm256_hadd_ps(crossings, crossings);
    crossings = _mm256_hadd_ps(crossings, crossings);
    int res = ElementAt(crossings, 0) + ElementAt(crossings, 4);
    int startIndex = ((ilength - 1) >> 3) << 3;
    float valpre = input[startIndex];
    for (int i = startIndex + 1; i < ilength; i++) {
      float val = input[i];
      if (valpre * val < 0 || valpre == 0) {
        res++;
      }
      valpre = val;
    }
    if (valpre == 0) {
      res++;
    }
    return res;
  } else {
#elif defined(__ARM_NEON__)
    uint32x4_t crossings = vdupq_n_u32(0);
    const float32x4_t zeros = vdupq_n_f32(0.f);
    const float32x4_t ones = vdupq_n_f32(1.f);
    for (int i = 0; i < ilength - 4; i += 4) {
      float32x4_t vecpre = vld1q_f32(input + i);
      float32x4_t zerocheck =  vceqq_f32(vecpre, zeros);
      float32x4_t vec = vld1q_f32(input + i + 1);
      float32x4_t tmp = vmulq_f32(vecpre, vec);
      uint32x4_t cmpres = vcltq_f32(tmp, zeros);
      cmpres = vorrq_u32(tmp, zerocheck);
      crossings = vaddq_u32(crossings, cmpres);
    }
    uint64x2_t crossings64 = vpaddlq_u32(crossings, crossings);
    int res = vgetq_lane_u64(crossings64, 0) + vgetq_lane_u64(crossings64, 1);
    int startIndex = ((ilength - 1) >> 2) << 2;
    float valpre = input[startIndex];
    for (int i = startIndex + 1; i < ilength; i++) {
      float val = input[i];
      if (valpre * val < 0 || valpre == 0) {
        res++;
      }
      valpre = val;
    }
    if (valpre == 0) {
      res++;
    }
    return res;
  } else {
#else
  } {
#endif
    int res = 0;
    float valpre = input[0];
    for (int i = 1; i < ilength; i++) {
      float val = input[i];
      if (valpre * val < 0 || valpre == 0) {
        res++;
      }
      valpre = val;
    }
    if (valpre == 0) {
      res++;
    }
    return res;
  }
}

REGISTER_TRANSFORM(ZeroCrossings);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
