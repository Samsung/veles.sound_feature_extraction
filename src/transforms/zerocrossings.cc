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
#include <simd/instruction_set.h>

namespace sound_feature_extraction {
namespace transforms {

int ZeroCrossingsF::DoInternal(bool simd, const float* input,
                               size_t length) const noexcept {
  int ilength = length;
  if (simd) {
#ifdef __AVX__
    __m256 crossings = _mm256_setzero_ps();
    const __m256 zeros = _mm256_setzero_ps();
    const __m256 ones = _mm256_set1_ps(1.f);
    for (int i = 0; i < ilength - 8; i += 8) {
      __m256 vecpre = _mm256_load_ps(input + i);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
      __m256 zerocheck =  _mm256_cmp_ps(vecpre, zeros, _CMP_EQ_OQ);
      __m256 vec = _mm256_loadu_ps(input + i + 1);
      __m256 tmp = _mm256_mul_ps(vecpre, vec);
      tmp = _mm256_cmp_ps(tmp, zeros, _CMP_LT_OQ);
#pragma GCC diagnostic pop
      tmp = _mm256_or_ps(tmp, zerocheck);
      tmp = _mm256_blendv_ps(zeros, ones, tmp);
      crossings = _mm256_add_ps(crossings, tmp);
    }
    crossings = _mm256_hadd_ps(crossings, crossings);
    crossings = _mm256_hadd_ps(crossings, crossings);
    int res = _mm256_get_ps(crossings, 0) + _mm256_get_ps(crossings, 4);
    int startIndex = (ilength - 1) & ~0x7;
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
    uint32x4_t crossings = vdupq_n_u32(0), ones = vdupq_n_u32(1);
    const float32x4_t zeros = vdupq_n_f32(0.f);
    for (int i = 0; i < ilength - 4; i += 4) {
      float32x4_t vecpre = vld1q_f32(input + i);
      uint32x4_t zerocheck =  vceqq_f32(vecpre, zeros);
      float32x4_t vec = vld1q_f32(input + i + 1);
      float32x4_t tmp = vmulq_f32(vecpre, vec);
      uint32x4_t cmpres = vcltq_f32(tmp, zeros);
      cmpres = vorrq_u32(cmpres, zerocheck);
      cmpres = vandq_u32(cmpres, ones);
      crossings = vaddq_u32(crossings, cmpres);
    }
    uint64x2_t crossings64 = vpaddlq_u32(crossings);
    int res = vgetq_lane_u64(crossings64, 0) + vgetq_lane_u64(crossings64, 1);
    int startIndex = (ilength - 1) & ~0x3;
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

int ZeroCrossings16::DoInternal(bool simd, const int16_t* input,
                                size_t length) const noexcept {
  int ilength = length;
  if (simd) {
#ifdef __AVX__
#ifdef __AVX2__
#error TODO(v.markovtsev): AVX2 adds integer operations, implement Do properly
#endif
    __m128i crossings = _mm_setzero_si128();
    const __m128i zeros = _mm_setzero_si128();
    const __m128i ones = _mm_set1_epi16(1);
    for (int i = 0; i < ilength - 8; i += 8) {
      __m128i vecpre = _mm_loadu_si128(
          reinterpret_cast<const __m128i*>(input + i));
      __m128i zerocheck =  _mm_cmpeq_epi16(vecpre, zeros);
      __m128i vec = _mm_loadu_si128(
          reinterpret_cast<const __m128i*>(input + i + 1));
      __m128i tmp = _mm_xor_si128(vecpre, vec);
      tmp = _mm_cmplt_epi16(tmp, ones);
      tmp = _mm_xor_si128(tmp, zerocheck);
      tmp = _mm_and_si128(tmp, ones);
      __m128i tmplo = _mm_unpacklo_epi16(tmp, zeros);
      __m128i tmphi = _mm_unpackhi_epi16(tmp, zeros);
      crossings = _mm_add_epi32(crossings, tmplo);
      crossings = _mm_add_epi32(crossings, tmphi);
    }
    crossings = _mm_hadd_epi32(crossings, crossings);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
    int res = _mm_extract_epi32(crossings, 0) + _mm_extract_epi32(crossings, 2);
#pragma GCC diagnostic pop
    int startIndex = ((ilength - 1) >> 3) << 3;
    int16_t valpre = input[startIndex];
    for (int i = startIndex + 1; i < ilength; i++) {
      int16_t val = input[i];
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
    uint16x8_t ones = vdupq_n_u16(1);
    const int16x8_t zeros = vdupq_n_s16(0);
    for (int i = 0; i < ilength - 8; i += 8) {
      int16x8_t vecpre = vld1q_s16(input + i);
      uint16x8_t zerocheck =  vceqq_s16(vecpre, zeros);
      int16x8_t vec = vld1q_s16(input + i + 1);
      int16x8_t tmp = veorq_s16(vecpre, vec);
      uint16x8_t cmpres = vcleq_s16(tmp, zeros);
      cmpres = veorq_u16(cmpres, zerocheck);
      cmpres = vandq_u16(cmpres, ones);
      crossings = vpadalq_u16(crossings, cmpres);
    }
    uint64x2_t crossings64 = vpaddlq_u32(crossings);
    int res = vgetq_lane_u64(crossings64, 0) + vgetq_lane_u64(crossings64, 1);
    int startIndex = ((ilength - 1) >> 3) << 3;
    int16_t valpre = input[startIndex];
    for (int i = startIndex + 1; i < ilength; i++) {
      int16_t val = input[i];
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
    int16_t valpre = input[0];
    for (int i = 1; i < ilength; i++) {
      int16_t val = input[i];
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

REGISTER_TRANSFORM(ZeroCrossingsF);
REGISTER_TRANSFORM(ZeroCrossings16);

}  // namespace transforms
}  // namespace sound_feature_extraction
