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

#define max(a,b) \
   ({ \
     __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; \
   })

void wavelet_apply(const float *__restrict src, size_t length,
                   float *__restrict desthi, float *__restrict destlo) {
  const int order = 8;

  assert(length >= (size_t)order);
  assert(length % 2 == 0);
  assert(src && desthi && destlo);

  float highpassC[order] __attribute__ ((aligned (32))),
        lowpassC[order] __attribute__ ((aligned (32)));
  for (int i = 0; i < order; i++) {
    float val =  DaubechiesF[order / 2][i];
    lowpassC[i] = val;
    highpassC[order - i - 1] = (i & 1) ? -val : val;
  }

#ifndef __AVX__
  float srcExt[(order - 2) * 2];
  memcpy(srcExt, src + length - (order - 2),
         (order - 2) * sizeof(float));
  memcpy(srcExt + order - 2, src, (order - 2) * sizeof(float));
#else
  float srcExt[(order - 2) * 2 * 4];
  // TODO(v.markovtsev): init srcExt
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
    for (int j = di, step = 0; j >= 0 && step < 4; j -= 6, step++) {
      desthi[j] = reshi;
      destlo[j] = reslo;
    }
  }
  for (int i = 0, di = (length - 6) / 2; i < (int)length; i += 2, di++) {
    __m256 srcvec = _mm256_load_ps(srcExt + i * 4);
    __m256 vechi = _mm256_dp_ps(srcvec, hpvec, 0xFF);
    __m256 veclo = _mm256_dp_ps(srcvec, lpvec, 0xFF);
    float reshi = vechi[0] + vechi[4];
    float reslo = veclo[0] + veclo[4];
    // Make 4 backward copies of the result
    for (int j = di, step = 0; j >= 0 && step < 4; j -= 6, step++) {
      desthi[j] = reshi;
      destlo[j] = reslo;
    }
  }
#elif defined(__ARM_NEON__)
  const float32x4_t hivec1 = vld1q_f32(highpass);
  const float32x4_t hivec2 = vld1q_f32(&highpass[4]);
  const float32x4_t lovec1 = vld1q_f32(lowpass);
  const float32x4_t lovec2 = vld1q_f32(&lowpass[4]);
  for (int i = 0, di = 0; i < (int)length - 6; i += 2, di++) {
    float32x4_t srcvec = vld1q_f32(src + i);

    float32x4_t vechi1 = vmulq_f32(srcvec, hivec1);
    float32x4_t vechi2 = vmulq_f32(srcvec, hivec2);
    float32x4_t veclo1 = vmulq_f32(srcvec, lovec1);
    float32x4_t veclo2 = vmulq_f32(srcvec, lovec2);

    float32x4_t vechiadd = vaddq_f32(vechi1, vechi2);
    float32x2_t tmpvechi = vadd_f32(vget_high_f32(vechiadd),
                                    vget_low_f32(vechiadd));
    float32x2_t resvechi = vpadd_f32(tmpvechi, tmpvechi);
    float reshi = vget_lane_f32(resvechi, 0);

    float32x4_t vecloadd = vaddq_f32(veclo1, veclo2);
    float32x2_t tmpveclo = vadd_f32(vget_high_f32(vecloadd),
                                    vget_low_f32(vecloadd));
    float32x2_t resveclo = vpadd_f32(tmpveclo, tmpveclo);
    float reslo = vget_lane_f32(resveclo, 0);

    desthi[di] = reshi;
    destlo[di] = reslo;
  }
  for (int i = 0, di = (length - 6) / 2; i < (int)length - 6; i += 2, di++) {
    float reshi = .0f, reslo = .0f;
    for (int j = 0; j < order; j++) {
      float srcval = src[i + j];
      reshi += highpass[j] * srcval;
      reslo += lowpass[j] * srcval;
    }
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#else
  for (int i = 0, di = 0; i < (int)length - (order - 2); i += 2, di++) {
    float reshi = .0f, reslo = .0f;
    for (int j = 0; j < order; j++) {
      float srcval = src[i + j];
      reshi += highpass[j] * srcval;
      reslo += lowpass[j] * srcval;
    }
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
  for (int i = 0, di = (length - (order - 2)) / 2; i < (int)length - (order - 2);
       i += 2, di++) {
    float reshi = .0f, reslo = .0f;
    for (int j = 0; j < order; j++) {
      float srcval = src[i + j];
      reshi += highpass[j] * srcval;
      reslo += lowpass[j] * srcval;
    }
    desthi[di] = reshi;
    destlo[di] = reslo;
  }
#endif
}
