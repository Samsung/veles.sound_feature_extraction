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
#define WAVELET_INTERNAL_USE
#include "src/primitives/coiflets.h"
#include "src/primitives/daubechies.h"
#include "src/primitives/memory.h"
#include "src/primitives/symlets.h"

#define max(a,b) \
   ({ \
     __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; \
   })

static INLINE void check_length(size_t length) {
  assert(length > 0);
  assert(length % 2 == 0);
}

static INLINE size_t aligned_length(size_t length, size_t alignment) {
  size_t ex = length % alignment;
  if (ex == 0) {
    return length;
  }
  return length + alignment - ex;
}

static INLINE NOTNULL(2, 4) void wavelet_prepare_array_memcpy(
    int order, const float *src, size_t length, float *res) {
  check_length(length);

  if (res != src) {
    memcpy(res, src, length * sizeof(float));
  }

  if (length > 8) {
    size_t alength = aligned_length(length, 8);
    int copySize = (alength - 8) * sizeof(float);
    memcpy(res + alength,          src + 2, copySize);
    if (order > 4) {
      memcpy(res + alength * 2 -  8, src + 4, copySize);
      memcpy(res + alength * 3 - 16, src + 6, copySize);
    }
  }
}

int wavelet_validate_order(WaveletType type, int order) {
  size_t uorder = (size_t)order;
  switch (type) {
    case WAVELET_TYPE_DAUBECHIES:
      return (uorder <= sizeof(kDaubechiesF[0]) / sizeof(kDaubechiesF[0][0])) &&
             (uorder % 2 == 0);
    case WAVELET_TYPE_COIFLET:
      return (uorder <= sizeof(kCoifletsF[0]) / sizeof(kCoifletsF[0][0])) &&
             (uorder % 6 == 0);
    case WAVELET_TYPE_SYMLET:
      return (uorder <= sizeof(kSymletsF[0]) / sizeof(kSymletsF[0][0])) &&
             (uorder % 2 == 0);
    default:
      return 0;
  }
}

float *wavelet_prepare_array(int order
#ifndef __AVX__
                            UNUSED
#endif
                            , const float *src, size_t length
#ifndef __AVX__
                             UNUSED
#endif
) {
  check_length(length);
#ifndef __AVX__
  float *res = mallocf(length);
  memcpy(res, src, length * sizeof(src[0]));
#else
  size_t alength = aligned_length(length, 8);
  float *res = mallocf(alength * (order > 4? 4 : 2));
  wavelet_prepare_array_memcpy(order, src, length, res);
#endif
  return res;
}

float *wavelet_allocate_destination(int order
#ifndef __AVX__
                            UNUSED
#endif
                            , size_t sourceLength) {
  check_length(sourceLength);
  assert(sourceLength % 4 == 0);

#ifndef __AVX__
  float *res = mallocf(sourceLength / 2);
#else
  size_t alength = aligned_length(sourceLength, 8);
  float *res = mallocf(alength * (order > 4? 2 : 1));
#endif
  return res;
}

void wavelet_recycle_source(int order
#ifndef __AVX__
                            UNUSED
#endif
                            , float *src, size_t length,
                            float **desthihi, float **desthilo,
                            float **destlohi, float **destlolo) {
  if (length == 0 || length % 4 != 0) {
    *desthihi = NULL;
    *desthilo = NULL;
    *destlohi = NULL;
    *destlolo = NULL;
    return;
  }

#ifndef __AVX__
  int lq = length / 4;
#else
  int lq = aligned_length(length, 8) / (order > 4? 1 : 2);
  if (length < 16) {
    lq = length / 4;
  }
#endif
  *desthihi = src;
  *desthilo = src + lq;
  *destlohi = src + lq * 2;
  *destlolo = src + lq * 3;
}

static INLINE NOTNULL(3, 4) void initialize_highpass_lowpass(
    WaveletType type, int order, float *highpass, float *lowpass) {
  size_t uorder = (size_t)order;
  for (int i = 0; i < order; i++) {
    float val = .0f;
    switch (type) {
      case WAVELET_TYPE_DAUBECHIES:
        assert(uorder <= sizeof(kDaubechiesF[0]) / sizeof(kDaubechiesF[0][0]) &&
               uorder % 2 == 0 &&
               "Supported Daubechies orders are 2..76 (even numbers only)");
        val =  kDaubechiesF[order / 2 - 1][i];
        break;
      case WAVELET_TYPE_COIFLET:
        assert(uorder <= sizeof(kCoifletsF[0]) / sizeof(kCoifletsF[0][0]) &&
               uorder % 6 == 0 &&
               "Supported Coiflet orders are 6, 12, 18, 24 and 30");
        val = kCoifletsF[order / 6 - 1][i];
        break;
      case WAVELET_TYPE_SYMLET:
        assert(uorder <= sizeof(kSymletsF[0]) / sizeof(kSymletsF[0][0]) &&
               uorder % 2 == 0 &&
               "Supported Daubechies orders are 2..76 (even numbers only)");
        val = kSymletsF[order / 2 - 1][i];
        break;
    }
    lowpass[i] = val;
    highpass[order - i - 1] = (i & 1) ? val : -val;
  }
}

void wavelet_apply_na(WaveletType type, int order,
                      const float *__restrict src, size_t length,
                      float *__restrict desthi, float *__restrict destlo) {
  check_length(length);
  assert(src && desthi && destlo);

  int ilength = (int)length;
  float highpassC[order], lowpassC[order];
  initialize_highpass_lowpass(type, order, highpassC, lowpassC);

  if (ilength != order) {
    for (int i = 0, di = 0; i < ilength; i += 2, di++) {
      float reshi = .0f, reslo = .0f;
      for (int j = 0; j < order; j++) {
        int index = i + j;
        float srcval = src[index < ilength? index : index % ilength];
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
          float srcval = src[index < 8? index : index % 8];
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
          float srcval = src[index < order? index : index % order];
          reshi += highpassC[j] * srcval;
          reslo += lowpassC[j] * srcval;
        }
        desthi[di] = reshi;
        destlo[di] = reslo;
      }
    }
  }
}

#ifdef __AVX__
#define ALIGN_ORDER(order) (order % 8 == 0? order : (order / 8 + 1) * 8)

#define DECLARE_PASSC(order) \
  float highpassC[ALIGN_ORDER(order)] __attribute__ ((aligned (32))), \
        lowpassC[ALIGN_ORDER(order)] __attribute__ ((aligned (32)))
#else
#define DECLARE_PASSC(order) float highpassC[order], lowpassC[order]

#define align_complement_f32(x) 0
#endif

static void wavelet_apply4(WaveletType type,
                           const float *__restrict src, size_t length,
                           float *__restrict desthi,
                           float *__restrict destlo) {
#ifdef SIMD
  check_length(length);
  assert(src && desthi && destlo);

  if (align_complement_f32(src) != 0 ||
#ifdef __AVX__
      length < 8
#elif defined(__ARM_NEON__)
      length < 4
#endif
  ) {
    wavelet_apply_na(type, 4, src, length, desthi, destlo);
    return;
  }

  int ilength = (int)length;
  DECLARE_PASSC(4);
  initialize_highpass_lowpass(type, 4, highpassC, lowpassC);

#ifdef __AVX__
  memcpy(&highpassC[4], &highpassC[0], sizeof(highpassC) / 2);
  memcpy(&lowpassC[4], &lowpassC[0], sizeof(lowpassC) / 2);

  const __m256 hpvec = _mm256_load_ps(highpassC);
  const __m256 lpvec = _mm256_load_ps(lowpassC);
  size_t alength = aligned_length(length, 8);
  for (int i = 0, di = 0; i < ilength - 6; i += 8, di += 4) {
    __m256 srcvec1 = _mm256_load_ps(src + i);
    __m256 srcvec2 = _mm256_load_ps(src + i + alength);
    __m256 vechi1 = _mm256_dp_ps(srcvec1, hpvec, 0xFF);
    __m256 veclo1 = _mm256_dp_ps(srcvec1, lpvec, 0xFF);
    __m256 vechi2 = _mm256_dp_ps(srcvec2, hpvec, 0xFF);
    __m256 veclo2 = _mm256_dp_ps(srcvec2, lpvec, 0xFF);
    desthi[di] = vechi1[0];
    destlo[di] = veclo1[0];
    desthi[di + 1] = vechi2[0];
    destlo[di + 1] = veclo2[0];
    desthi[di + 2] = vechi1[4];
    destlo[di + 2] = veclo1[4];
    desthi[di + 3] = vechi2[4];
    destlo[di + 3] = veclo2[4];
  }
#elif defined(__ARM_NEON__)
  const float32x4_t hivec = vld1q_f32(highpassC);
  const float32x4_t lovec = vld1q_f32(lowpassC);
  for (int i = 0, di = 0; i < ilength - 2; i += 2, di++) {
    float32x4_t srcvec = vld1q_f32(src + i);

    float32x4_t vechiadd = vmulq_f32(srcvec, hivec);
    float32x4_t vecloadd = vmulq_f32(srcvec, lovec);

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
  // Finish with the extended end
#ifdef __AVX__
  for (int i = ilength - 6, di = (ilength - 6) / 2;
#elif defined(__ARM_NEON__)
  for (int i = ilength - 2, di = (ilength - 2) / 2;
#endif
       i < ilength; i += 2, di++) {
    float reshi = .0f, reslo = .0f;
    for (int j = 0; j < 4; j++) {
      int index = i + j;
      float srcval = src[index < ilength? index : index % ilength];
      reshi += highpassC[j] * srcval;
      reslo += lowpassC[j] * srcval;
    }
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#ifdef __AVX__
  if (length >= 16) {
    wavelet_prepare_array_memcpy(4, desthi, length / 2, desthi);
    wavelet_prepare_array_memcpy(4, destlo, length / 2, destlo);
  }
#endif  // #ifdef __AVX__
#else  // #ifdef SIMD
  wavelet_apply_na(type, 4, src, length, desthi, destlo);
#endif
}

static void wavelet_apply6(WaveletType type,
                           const float *__restrict src, size_t length,
                           float *__restrict desthi,
                           float *__restrict destlo) {
#ifdef SIMD
  check_length(length);
  assert(src && desthi && destlo);
  assert(align_complement_f32(src) == 0);

  if (align_complement_f32(src) != 0 || length < 8) {
    wavelet_apply_na(type, 6, src, length, desthi, destlo);
    return;
  }

  int ilength = (int)length;
  DECLARE_PASSC(6);
  initialize_highpass_lowpass(type, 6, highpassC, lowpassC);

#ifdef __AVX__
  highpassC[6] = .0f;
  highpassC[7] = .0f;
  lowpassC[6] = .0f;
  lowpassC[7] = .0f;
  const __m256 hpvec = _mm256_load_ps(highpassC);
  const __m256 lpvec = _mm256_load_ps(lowpassC);
  size_t alength = aligned_length(length, 8);
  for (int i = 0, di = 0; i < ilength - 6; i += 2, di++) {
    int ex = (i / 2) % 4;
    int offset = i + (ex > 0? ex * (alength - 10) + 8 : 0);
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
  const float32x2_t hivec2 = vld1_f32(&highpassC[4]);
  const float32x4_t lovec1 = vld1q_f32(lowpassC);
  const float32x2_t lovec2 = vld1_f32(&lowpassC[4]);
  for (int i = 0, di = 0; i < ilength - 4; i += 2, di++) {
    float32x4_t srcvec1 = vld1q_f32(src + i);
    float32x2_t srcvec2 = vld1_f32(src + i + 4);

    float32x4_t vechiadd1 = vmulq_f32(srcvec1, hivec1);
    float32x4_t vecloadd1 = vmulq_f32(srcvec1, lovec1);
    float32x2_t vechiadd2 = vmul_f32(srcvec2, hivec2);
    float32x2_t vecloadd2 = vmul_f32(srcvec2, lovec2);

    float32x2_t vechipair = vadd_f32(vget_high_f32(vechiadd1),
                                     vget_low_f32(vechiadd1));
    float32x2_t veclopair = vadd_f32(vget_high_f32(vechiadd2),
                                     vget_low_f32(vechiadd2));
    vadd_f32(vechipair, vechiadd2);
    vadd_f32(veclopair, vecloadd2);

    float32x2_t vecres = vpadd_f32(vechipair, veclopair);

    float reshi = vget_lane_f32(vecres, 0);
    float reslo = vget_lane_f32(vecres, 1);

    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#else
#error This SIMD variant is not supported.
#endif  // #elif defined(__ARM_NEON__)
  // Finish with the extended end
#ifdef __AVX__
  for (int i = ilength - 6, di = (ilength - 6) / 2;
#elif defined(__ARM_NEON__)
  for (int i = ilength - 4, di = (ilength - 4) / 2;
#endif
       i < ilength; i += 2, di++) {
    float reshi = .0f, reslo = .0f;
    for (int j = 0; j < 6; j++) {
      int index = i + j;
      float srcval = src[index < ilength? index : index % ilength];
      reshi += highpassC[j] * srcval;
      reslo += lowpassC[j] * srcval;
    }
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#ifdef __AVX__
  if (length >= 16) {
    wavelet_prepare_array_memcpy(6, desthi, length / 2, desthi);
    wavelet_prepare_array_memcpy(6, destlo, length / 2, destlo);
  }
#endif  // #ifdef __AVX__
#else  // #ifdef SIMD
  wavelet_apply_na(type, 6, src, length, desthi, destlo);
#endif
}

static void wavelet_apply8(WaveletType type,
                           const float *__restrict src, size_t length,
                           float *__restrict desthi,
                           float *__restrict destlo) {
#ifdef SIMD
  check_length(length);
  assert(src && desthi && destlo);
  if (align_complement_f32(src) != 0 || length < 8) {
    wavelet_apply_na(type, 8, src, length, desthi, destlo);
    return;
  }

  int ilength = (int)length;
  DECLARE_PASSC(8);
  initialize_highpass_lowpass(type, 8, highpassC, lowpassC);

#ifdef __AVX__
  const __m256 hpvec = _mm256_load_ps(highpassC);
  const __m256 lpvec = _mm256_load_ps(lowpassC);
  size_t alength = aligned_length(length, 8);
  for (int i = 0, di = 0; i < ilength - 6; i += 2, di++) {
    int ex = (i / 2) % 4;
    int offset = i + (ex > 0? ex * (alength - 10) + 8 : 0);
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
    float32x4_t srcvec1 = vld1q_f32(src + i);
    float32x4_t srcvec2 = vld1q_f32(src + i + 4);

    float32x4_t vechiadd = vmulq_f32(srcvec1, hivec1);
    float32x4_t vecloadd = vmulq_f32(srcvec1, lovec1);
    vechiadd = vfmaq_f32(vechiadd, srcvec2, hivec2);
    vecloadd = vfmaq_f32(vecloadd, srcvec2, lovec2);

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
  // Finish with the extended end
  for (int i = ilength - 6, di = (ilength - 6) / 2;
       i < ilength; i += 2, di++) {
    float reshi = .0f, reslo = .0f;
    for (int j = 0; j < 8; j++) {
      int index = i + j;
      float srcval = src[index < ilength? index : index % ilength];
      reshi += highpassC[j] * srcval;
      reslo += lowpassC[j] * srcval;
    }
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#ifdef __AVX__
  if (length >= 16) {
    wavelet_prepare_array_memcpy(8, desthi, length / 2, desthi);
    wavelet_prepare_array_memcpy(8, destlo, length / 2, destlo);
  }
#endif  // #ifdef __AVX__
#else  // #ifdef SIMD
  wavelet_apply_na(type, 8, src, length, desthi, destlo);
#endif
}

static void wavelet_apply12(WaveletType type,
                            const float *__restrict src, size_t length,
                            float *__restrict desthi,
                            float *__restrict destlo) {
#ifdef SIMD
  check_length(length);
  assert(src && desthi && destlo);
  assert(align_complement_f32(src) == 0);

  if (align_complement_f32(src) != 0 ||
#ifdef __AVX__
      length < 16
#elif defined(__ARM_NEON__)
      length < 12
#endif
  ) {
    wavelet_apply_na(type, 12, src, length, desthi, destlo);
    return;
  }

  int ilength = (int)length;
  DECLARE_PASSC(12);
  initialize_highpass_lowpass(type, 12, highpassC, lowpassC);

#ifdef __AVX__
  highpassC[12] = .0f;
  highpassC[13] = .0f;
  highpassC[14] = .0f;
  highpassC[15] = .0f;
  lowpassC[12] = .0f;
  lowpassC[13] = .0f;
  lowpassC[14] = .0f;
  lowpassC[15] = .0f;
  const __m256 hpvec1 = _mm256_load_ps(highpassC);
  const __m256 lpvec1 = _mm256_load_ps(lowpassC);
  const __m256 hpvec2 = _mm256_load_ps(&highpassC[8]);
  const __m256 lpvec2 = _mm256_load_ps(&lowpassC[8]);
  size_t alength = aligned_length(length, 8);
  for (int i = 0, di = 0; i < ilength - 14; i += 2, di++) {
    int ex = (i / 2) % 4;
    int offset = i + (ex > 0? ex * (alength - 10) + 8 : 0);
    __m256 srcvec1 = _mm256_load_ps(src + offset);
    __m256 srcvec2 = _mm256_load_ps(src + offset + 8);
    __m256 vechi1 = _mm256_dp_ps(srcvec1, hpvec1, 0xFF);
    __m256 veclo1 = _mm256_dp_ps(srcvec1, lpvec1, 0xFF);
    __m256 vechi2 = _mm256_dp_ps(srcvec2, hpvec2, 0xFF);
    __m256 veclo2 = _mm256_dp_ps(srcvec2, lpvec2, 0xFF);
    __m256 vechi = _mm256_add_ps(vechi1, vechi2);
    __m256 veclo = _mm256_add_ps(veclo1, veclo2);
    float reshi = vechi[0] + vechi[4];
    float reslo = veclo[0] + veclo[4];
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#elif defined(__ARM_NEON__)
  const float32x4_t hivec1 = vld1q_f32(highpassC);
  const float32x4_t hivec2 = vld1q_f32(&highpassC[4]);
  const float32x4_t hivec3 = vld1q_f32(&highpassC[8]);
  const float32x4_t lovec1 = vld1q_f32(lowpassC);
  const float32x4_t lovec2 = vld1q_f32(&lowpassC[4]);
  const float32x4_t lovec3 = vld1q_f32(&lowpassC[8]);
  for (int i = 0, di = 0; i < ilength - 10; i += 2, di++) {
    float32x4_t srcvec1 = vld1q_f32(src + i);
    float32x4_t srcvec2 = vld1q_f32(src + i + 4);
    float32x4_t srcvec3 = vld1q_f32(src + i + 8);

    float32x4_t vechiadd = vmulq_f32(srcvec1, hivec1);
    float32x4_t vecloadd = vmulq_f32(srcvec1, lovec1);
    vechiadd = vfmaq_f32(vechiadd, srcvec2, hivec2);
    vecloadd = vfmaq_f32(vecloadd, srcvec2, lovec2);
    vechiadd = vfmaq_f32(vechiadd, srcvec3, hivec3);
    vecloadd = vfmaq_f32(vecloadd, srcvec3, lovec3);

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
  // Finish with the extended end
#ifdef __AVX__
  for (int i = ilength - 14, di = (ilength - 14) / 2;
#elif defined(__ARM_NEON__)
  for (int i = ilength - 10, di = (ilength - 10) / 2;
#endif
       i < ilength; i += 2, di++) {
    float reshi = .0f, reslo = .0f;
    for (int j = 0; j < 12; j++) {
      int index = i + j;
      float srcval = src[index < ilength? index : index % ilength];
      reshi += highpassC[j] * srcval;
      reslo += lowpassC[j] * srcval;
    }
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#ifdef __AVX__
  if (length >= 24) {
    wavelet_prepare_array_memcpy(12, desthi, length / 2, desthi);
    wavelet_prepare_array_memcpy(12, destlo, length / 2, destlo);
  }
#endif  // #ifdef __AVX__
#else  // #ifdef SIMD
  wavelet_apply_na(type, 12, src, length, desthi, destlo);
#endif
}

static void wavelet_apply16(WaveletType type,
                            const float *__restrict src, size_t length,
                            float *__restrict desthi,
                            float *__restrict destlo) {
#ifdef SIMD
  check_length(length);
  assert(src && desthi && destlo);
  assert(align_complement_f32(src) == 0);

  if (align_complement_f32(src) != 0 || length < 16) {
    wavelet_apply_na(type, 16, src, length, desthi, destlo);
    return;
  }

  int ilength = (int)length;
  DECLARE_PASSC(16);
  initialize_highpass_lowpass(type, 16, highpassC, lowpassC);

#ifdef __AVX__
  const __m256 hpvec1 = _mm256_load_ps(highpassC);
  const __m256 lpvec1 = _mm256_load_ps(lowpassC);
  const __m256 hpvec2 = _mm256_load_ps(&highpassC[8]);
  const __m256 lpvec2 = _mm256_load_ps(&lowpassC[8]);
  size_t alength = aligned_length(length, 8);
  for (int i = 0, di = 0; i < ilength - 14; i += 2, di++) {
    int ex = (i / 2) % 4;
    int offset = i + (ex > 0? ex * (alength - 10) + 8 : 0);
    __m256 srcvec1 = _mm256_load_ps(src + offset);
    __m256 srcvec2 = _mm256_load_ps(src + offset + 8);
    __m256 vechi1 = _mm256_dp_ps(srcvec1, hpvec1, 0xFF);
    __m256 veclo1 = _mm256_dp_ps(srcvec1, lpvec1, 0xFF);
    __m256 vechi2 = _mm256_dp_ps(srcvec2, hpvec2, 0xFF);
    __m256 veclo2 = _mm256_dp_ps(srcvec2, lpvec2, 0xFF);
    __m256 vechi = _mm256_add_ps(vechi1, vechi2);
    __m256 veclo = _mm256_add_ps(veclo1, veclo2);
    float reshi = vechi[0] + vechi[4];
    float reslo = veclo[0] + veclo[4];
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#elif defined(__ARM_NEON__)
  const float32x4_t hivec1 = vld1q_f32(highpassC);
  const float32x4_t hivec2 = vld1q_f32(&highpassC[4]);
  const float32x4_t hivec3 = vld1q_f32(&highpassC[8]);
  const float32x4_t hivec4 = vld1q_f32(&highpassC[12]);
  const float32x4_t lovec1 = vld1q_f32(lowpassC);
  const float32x4_t lovec2 = vld1q_f32(&lowpassC[4]);
  const float32x4_t lovec3 = vld1q_f32(&lowpassC[8]);
  const float32x4_t lovec4 = vld1q_f32(&lowpassC[12]);
  for (int i = 0, di = 0; i < ilength - 10; i += 2, di++) {
    float32x4_t srcvec1 = vld1q_f32(src + i);
    float32x4_t srcvec2 = vld1q_f32(src + i + 4);
    float32x4_t srcvec3 = vld1q_f32(src + i + 8);
    float32x4_t srcvec4 = vld1q_f32(src + i + 12);

    float32x4_t vechiadd = vmulq_f32(srcvec1, hivec1);
    float32x4_t vecloadd = vmulq_f32(srcvec1, lovec1);
    vechiadd = vfmaq_f32(vechiadd, srcvec2, hivec2);
    vecloadd = vfmaq_f32(vecloadd, srcvec2, lovec2);
    vechiadd = vfmaq_f32(vechiadd, srcvec3, hivec3);
    vecloadd = vfmaq_f32(vecloadd, srcvec3, lovec3);
    vechiadd = vfmaq_f32(vechiadd, srcvec4, hivec4);
    vecloadd = vfmaq_f32(vecloadd, srcvec4, lovec4);

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
  // Finish with the extended end
  for (int i = ilength - 14, di = (ilength - 14) / 2;
       i < ilength; i += 2, di++) {
    float reshi = .0f, reslo = .0f;
    for (int j = 0; j < 16; j++) {
      int index = i + j;
      float srcval = src[index < ilength? index : index % ilength];
      reshi += highpassC[j] * srcval;
      reslo += lowpassC[j] * srcval;
    }
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#ifdef __AVX__
  if (length >= 32) {
    wavelet_prepare_array_memcpy(16, desthi, length / 2, desthi);
    wavelet_prepare_array_memcpy(16, destlo, length / 2, destlo);
  }
#endif  // #ifdef __AVX__
#else  // #ifdef SIMD
  wavelet_apply_na(type, 16, src, length, desthi, destlo);
#endif
}

void wavelet_apply(WaveletType type, int order,
                   const float *__restrict src, size_t length,
                   float *__restrict desthi, float *__restrict destlo) {
  switch (order) {
    case 4:
      wavelet_apply4(type, src, length, desthi, destlo);
      break;
    case 6:
      wavelet_apply6(type, src, length, desthi, destlo);
      break;
    case 8:
      wavelet_apply8(type, src, length, desthi, destlo);
      break;
    case 12:
      wavelet_apply12(type, src, length, desthi, destlo);
      break;
    case 16:
      wavelet_apply16(type, src, length, desthi, destlo);
      break;
    default:
      wavelet_apply_na(type, order, src, length, desthi, destlo);
      break;
  }
}
