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

float *wavelet_scatter_array(const float *src, size_t length
#ifndef __AVX__
                             UNUSED
#endif
) {
  check_length(length);
#ifndef __AVX__
  return src;
#else
  float *res = mallocf(length * 4 + 6);
  res[0] = src[0];
  res[1] = src[1];
  float val = src[2];
  res[2] = val;
  res[8] = val;
  val = src[3];
  res[3] = val;
  res[9] = val;
  val = src[4];
  res[4] = val;
  res[10] = val;
  res[16] = val;
  val = src[5];
  res[5] = val;
  res[11] = val;
  res[17] = val;
  for (int i = 6; i < (int)length; i++) {
    int baseIndex = -18 + (i / 2) * 8 + (i % 2);
    float val = src[i];
    for (int j = 0; j < 4 * 6; j += 6) {
      res[baseIndex + j] = val;
    }
  }
  return res;
#endif
}

void wavelet_gather_array(float *src, size_t length
#ifndef __AVX__
                          UNUSED
#endif
) {
#ifndef __AVX__
  return src;
#else
  for (int i = 8; i < (int)length; i++) {
    src[i] = src[-18 + (i / 2) * 8 + i % 2];
  }
#endif
}

float *wavelet_allocate_destination(size_t sourceLength) {
  check_length(sourceLength);
#ifndef __AVX__
  float *res = mallocf(sourceLength / 2);
#else
  float *res = mallocf(sourceLength * 2 + 6);
#endif
  return res;
}

static void initialize_highpass_lowpass(int order,
                                        float *highpass, float *lowpass) {
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

  float highpassC[order], lowpassC[order];
  initialize_highpass_lowpass(order, highpassC, lowpassC);

  float srcExt[(order - 2) * 2];
  memcpy(srcExt, src + length - (order - 2),
         (order - 2) * sizeof(float));
  memcpy(srcExt + order - 2, src, (order - 2) * sizeof(float));

  for (int i = 0, di = 0; i < (int)length - (order - 2); i += 2, di++) {
    float reshi = .0f, reslo = .0f;
    for (int j = 0; j < order; j++) {
      float srcval = src[i + j];
      reshi += highpassC[j] * srcval;
      reslo += lowpassC[j] * srcval;
    }
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
  for (int i = 0, di = (length - (order - 2)) / 2; i < 6; i += 2, di++) {
    float reshi = .0f, reslo = .0f;
    for (int j = 0; j < order; j++) {
      float srcval = srcExt[i + j];
      reshi += highpassC[j] * srcval;
      reslo += lowpassC[j] * srcval;
    }
    desthi[di] = reshi;
    destlo[di] = reslo;
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

  float highpassC[order] __attribute__ ((aligned (32))),
        lowpassC[order] __attribute__ ((aligned (32)));
  initialize_highpass_lowpass(order, highpassC, lowpassC);

#ifndef __AVX__
  float srcExt[(order - 2) * 2];
  memcpy(srcExt, src + length - (order - 2),
         (order - 2) * sizeof(float));
  memcpy(srcExt + order - 2, src, (order - 2) * sizeof(float));
#else
  float srcExt[8 * 3] __attribute__ ((aligned (32)));
  memcpy(srcExt, src + 4 * length - 30, sizeof(srcExt));
  float a = src[0], b = src[1],
      c = src[2], d = src[3],
      e = src[4], f = src[5];
  srcExt[6] = a;
  srcExt[7] = b;
  srcExt[12] = a;
  srcExt[13] = b;
  srcExt[14] = c;
  srcExt[15] = d;
  srcExt[18] = a;
  srcExt[19] = b;
  srcExt[20] = c;
  srcExt[21] = d;
  srcExt[22] = e;
  srcExt[23] = f;
#endif

#ifdef __AVX__
  const __m256 hpvec = _mm256_load_ps(highpassC);
  const __m256 lpvec = _mm256_load_ps(lowpassC);
  for (int i = 0, di = 0; i < (int)length - 6; i += 2, di++) {
    __m256 srcvec = _mm256_load_ps(src + i * 4);
    __m256 vechi = _mm256_dp_ps(srcvec, hpvec, 0xFF);
    __m256 veclo = _mm256_dp_ps(srcvec, lpvec, 0xFF);
    float reshi = vechi[0] + vechi[4];
    float reslo = veclo[0] + veclo[4];
    // Make 4 backward copies of the result
    if (di >= 6) {
      int baseIndex = -18 + (di / 2) * 8 + (di % 2);
      for (int j = 0; j < 4 * 6; j += 6) {
        desthi[baseIndex + j] = reshi;
        destlo[baseIndex + j] = reslo;
      }
    } else {
      desthi[di] = reshi;
      destlo[di] = reslo;
      if (di >= 2) {
        desthi[di + 6] = reshi;
        destlo[di + 6] = reslo;
      }
      if (di >= 4) {
        desthi[di + 6 * 2] = reshi;
        destlo[di + 6 * 2] = reslo;
      }
    }
  }
  for (int i = 0, di = (length - 6) / 2; i < 6; i += 2, di++) {
    __m256 srcvec = _mm256_load_ps(srcExt + i * 4);
    __m256 vechi = _mm256_dp_ps(srcvec, hpvec, 0xFF);
    __m256 veclo = _mm256_dp_ps(srcvec, lpvec, 0xFF);
    float reshi = vechi[0] + vechi[4];
    float reslo = veclo[0] + veclo[4];

    // Make 4 backward copies of the result
    int baseIndex = -18 + (di / 2) * 8 + (di % 2);
    for (int j = 0; j < 4 * 6; j += 6) {
      desthi[baseIndex + j] = reshi;
      destlo[baseIndex + j] = reslo;
    }
  }
#elif defined(__ARM_NEON__)
  const float32x4_t hivec1 = vld1q_f32(highpassC);
  const float32x4_t hivec2 = vld1q_f32(&highpassC[4]);
  const float32x4_t lovec1 = vld1q_f32(lowpassC);
  const float32x4_t lovec2 = vld1q_f32(&lowpassC[4]);
  for (int i = 0, di = 0; i < (int)length - 6; i += 2, di++) {
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
  for (int i = 0, di = (length - 6) / 2; i < 6; i += 2, di++) {
    float reshi = .0f, reslo = .0f;
    for (int j = 0; j < order; j++) {
      float srcval = srcExt[i + j];
      reshi += highpass[j] * srcval;
      reslo += lowpass[j] * srcval;
    }
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#else
#error This SIMD variant is not supported.
#endif  // #elif defined(__ARM_NEON__)
#endif  // #ifndef SIMD
}
