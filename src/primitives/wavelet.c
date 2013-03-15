/*! @file wavelet.c
 *  @brief Wavelet low level functions.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include "src/primitives/wavelet.h"
#include <assert.h>
#include <string.h>
#include "src/primitives/arithmetic-inl.h"
#include "src/primitives/daubechies.h"
#include "src/primitives/memory.h"

#define max(a,b) \
   ({ \
     __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; \
   })

static INLINE void check_length(size_t length) {
  assert((length & (length - 1)) == 0 && "length must be a power of 2");
}

static INLINE NOTNULL(1, 3) void wavelet_prepare_array_memcpy(
    const float *src, size_t length, float *res) {
  assert(length % 8 == 0);
  assert(length >= 16);

  if (res != src) {
    memcpy(res, src, length * sizeof(float));
  }
  int copySize = (length - 8) * sizeof(float);
  memcpy(res + length,          src + 2, copySize);
  memcpy(res + length * 2 -  8, src + 4, copySize);
  memcpy(res + length * 3 - 16, src + 6, copySize);
}

float *wavelet_prepare_array(const float *src, size_t length
#ifndef __AVX__
                             UNUSED
#endif
) {
  check_length(length);
#ifndef __AVX__
  return src;
#else
  float *res = mallocf(length * 4 - 24);
  wavelet_prepare_array_memcpy(src, length, res);
  return res;
#endif
}

float *wavelet_allocate_destination(size_t sourceLength) {
  check_length(sourceLength);
  assert(sourceLength >= 16);

#ifndef __AVX__
  float *res = mallocf(sourceLength / 2);
#else
  float *res = mallocf(sourceLength * 2 - 24);
#endif
  return res;
}

static INLINE NOTNULL(2, 3) void initialize_highpass_lowpass(
    int order, float *highpass, float *lowpass) {
  for (int i = 0; i < order; i++) {
    float val =  DaubechiesF[order / 2][i];
    lowpass[i] = val;
    highpass[order - i - 1] = (i & 1) ? val : -val;
  }
}

void wavelet_apply_na(const float *__restrict src, size_t length, int order,
                      float *__restrict desthi, float *__restrict destlo) {
  assert(length >= (size_t)order);
  assert(length % 2 == 0);
  assert(src && desthi && destlo);

  int ilength = (int)length;
  float highpassC[order], lowpassC[order];
  initialize_highpass_lowpass(order, highpassC, lowpassC);

  if (ilength > order) {
    for (int i = 0, di = 0; i < ilength; i += 2, di++) {
      float reshi = .0f, reslo = .0f;
      for (int j = 0; j < order; j++) {
        int index = i + j;
        float srcval = src[index < ilength? index : index - ilength];
        reshi += highpassC[j] * srcval;
        reslo += lowpassC[j] * srcval;
      }
      desthi[di] = reshi;
      destlo[di] = reslo;
    }
  } else {
    if (order == 8) {
      for (int i = 0, di = 0; i < 8; i += 2, di++) {
        float reshi = .0f, reslo = .0f;
        for (int j = 0; j < 8; j++) {
          int index = i + j;
          float srcval = src[index < 8? index : index - 8];
          reshi += highpassC[j] * srcval;
          reslo += lowpassC[j] * srcval;
        }
        desthi[di] = reshi;
        destlo[di] = reslo;
      }
    } else {
      for (int i = 0, di = 0; i < order; i += 2, di++) {
        float reshi = .0f, reslo = .0f;
        for (int j = 0; j < order; j++) {
          int index = i + j;
          float srcval = src[index < order? index : index - order];
          reshi += highpassC[j] * srcval;
          reslo += lowpassC[j] * srcval;
        }
        desthi[di] = reshi;
        destlo[di] = reslo;
      }
    }
  }
}

void wavelet_apply(const float *__restrict src, size_t length,
                   float *__restrict desthi, float *__restrict destlo) {
#ifndef SIMD
  wavelet_apply_na(src, length, 8, desthi, destlo);
#else
  const int order = 8;

  assert(length >= (size_t)order);
  assert(length % 2 == 0);
  assert(src && desthi && destlo);

  int ilength = (int)length;
  float highpassC[order] __attribute__ ((aligned (32))),
        lowpassC[order] __attribute__ ((aligned (32)));
  initialize_highpass_lowpass(order, highpassC, lowpassC);

#ifdef __AVX__
  const __m256 hpvec = _mm256_load_ps(highpassC);
  const __m256 lpvec = _mm256_load_ps(lowpassC);
  for (int i = 0, di = 0; i < ilength - 6; i += 2, di++) {
    int ex = (i / 2) % 4;
    int offset = i + (ex > 0? ex * (length - 10) + 8 : 0);
    __m256 srcvec = _mm256_load_ps(src + offset);
    __m256 vechi = _mm256_dp_ps(srcvec, hpvec, 0xFF);
    __m256 veclo = _mm256_dp_ps(srcvec, lpvec, 0xFF);
    float reshi = vechi[0] + vechi[4];
    float reslo = veclo[0] + veclo[4];
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#elif defined(__ARM_NEON__)
  const float32x4_t hivec1 = vld1q_f32(highpassC);
  const float32x4_t hivec2 = vld1q_f32(&highpassC[4]);
  const float32x4_t lovec1 = vld1q_f32(lowpassC);
  const float32x4_t lovec2 = vld1q_f32(&lowpassC[4]);
  for (int i = 0, di = 0; i < ilength - 6; i += 2, di++) {
    float32x4_t srcvec = vld1q_f32(src + i);

    float32x4_t vechitmp = vmulq_f32(srcvec, hivec1);
    float32x4_t veclotmp = vmulq_f32(srcvec, lovec1);
    float32x4_t vechiadd = vfmaq_f32(vechitmp, srcvec, hivec2);
    float32x4_t vecloadd = vfmaq_f32(veclotmp, srcvec, lovec2);

    float32x2_t vechipair = vadd_f32(vget_high_f32(vechiadd),
                                     vget_low_f32(vechiadd));
    float32x2_t veclopair = vadd_f32(vget_high_f32(vecloadd),
                                     vget_low_f32(vecloadd));

    float32x2_t vecres = vpadd_f32(vechipair, veclopair);

    float reshi = vget_lane_f32(vecres, 0);
    float reslo = vget_lane_f32(vecres, 1);

    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#else
#error This SIMD variant is not supported.
#endif  // #elif defined(__ARM_NEON__)
#endif  // #ifndef SIMD
  // Finish with the extended end
  for (int i = ilength - 6, di = (ilength - 6) / 2; i < ilength; i += 2, di++) {
    float reshi = .0f, reslo = .0f;
    for (int j = 0; j < 8; j++) {
      int index = i + j;
      float srcval = src[index < ilength? index : index - ilength];
      reshi += highpassC[j] * srcval;
      reslo += lowpassC[j] * srcval;
    }
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#ifdef __AVX__
  wavelet_prepare_array_memcpy(desthi, length / 2, desthi);
  wavelet_prepare_array_memcpy(destlo, length / 2, destlo);
#endif
}
