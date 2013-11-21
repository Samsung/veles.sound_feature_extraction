/*! @file lpc.h
 *  @brief Linear Prediction Coefficients (LPC) calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 *  Originally, this file was lpc.c from Speex source code. It was nearly 100%
 *  rewritten, so that no additional copyright notice is needed.
 */

#include "src/primitives/lpc.h"
#include <assert.h>
#include <math.h>
#include <simd/instruction_set.h>

float ldr_lpc(int simd, const float *ac, int length, float *lpc) {
#if !defined(__AVX__) && !defined(__ARM_NEON__)
  simd = 0;
#endif
  if (ac[0] == 0) {
     for (int i = 0; i < length - 1; i++) {
       lpc[i] = 0;
     }
     return 0;
  }

  float error = ac[0];
  for (int i = 0; i < length - 1; i++) {
    /* Sum up this iteration's reflection coefficients to calculate lambda */
    float rr = -ac[i + 1];
    if (!simd ||
#ifdef __AVX__
        i < 8
#elif defined(__ARM_NEON__)
        i < 4
#else
        0
#endif
    ) {
      for (int j = 0; j < i; j++) {
         rr -= lpc[j] * ac[i - j];
      }
    } else {
#ifdef __AVX__
        __m256 accum = _mm256_setzero_ps();
        for (int j = 0; j < i - 7; j += 8) {
          __m256 lpc_vec = _mm256_loadu_ps(lpc + j);
          __m256 ac_vec = _mm256_loadu_ps(ac + i - j - 7);
          ac_vec = _mm256_permute2f128_ps(ac_vec, ac_vec, 1);
          ac_vec = _mm256_permute_ps(ac_vec, 27);
          __m256 mulres = _mm256_mul_ps(lpc_vec, ac_vec);
          accum = _mm256_add_ps(accum, mulres);
        }
        accum = _mm256_hadd_ps(accum, accum);
        accum = _mm256_hadd_ps(accum, accum);
        rr -= _mm256_get_ps(accum, 0) + _mm256_get_ps(accum, 4);
        for (int j = (i & ~0x7); j < i; j++) {
           rr -= lpc[j] * ac[i - j];
        }
#elif defined(__ARM_NEON__)
      float32x4_t accum = vdupq_n_f32(0.f);
      for (int j = 0; j < i - 3; j += 4) {
        float32x4_t lpc_vec = vld1q_f32(lpc + j);
        float32x4_t ac_vec = vld1q_f32(ac + i - j - 3);
        ac_vec = vrev64q_f32(ac_vec);
        ac_vec = vcombine_f32(vget_high_f32(ac_vec), vget_low_f32(ac_vec));
        accum = vmlaq_f32(accum, lpc_vec, ac_vec);
      }
      float32x2_t accum2 = vpadd_f32(vget_high_f32(accum),
                                     vget_low_f32(accum));
      rr -= vget_lane_f32(accum2, 0) + vget_lane_f32(accum2, 1);
      for (int j = (i & ~0x3); j < i; j++) {
        rr -= lpc[j] * ac[i - j];
      }
#else
#endif
    }
    float lambda = rr / error;

    /*  Update LPC coefficients and total error */
    if (!simd) {
      for (int j = 0; j < i/2; j++) {
        int j_supp  = i - 1 - j;
        float tmp  = lpc[j];
        lpc[j]   += lambda * lpc[j_supp];
        lpc[j_supp] += lambda * tmp;
      }
    } else {
#ifdef __AVX__
      const __m256 lambda_vec = _mm256_set1_ps(lambda);
      for (int j = 0; j < i/2 - 7; j += 8) {
        int j_supp  = i - j - 8;
        __m256 lpc_vec = _mm256_loadu_ps(lpc + j);
        __m256 lpc_supp_vec = _mm256_loadu_ps(lpc + j_supp);

        __m256 lpc_supp_vec_rev = _mm256_permute2f128_ps(lpc_supp_vec,
                                                         lpc_supp_vec, 1);
        lpc_supp_vec_rev = _mm256_permute_ps(lpc_supp_vec_rev, 27);
        __m256 lpc_vec_rev = _mm256_permute2f128_ps(lpc_vec, lpc_vec, 1);
        lpc_vec_rev = _mm256_permute_ps(lpc_vec_rev, 27);

        lpc_supp_vec_rev = _mm256_mul_ps(lpc_supp_vec_rev, lambda_vec);
        lpc_vec_rev = _mm256_mul_ps(lpc_vec_rev, lambda_vec);
        lpc_vec = _mm256_add_ps(lpc_vec, lpc_supp_vec_rev);
        lpc_supp_vec = _mm256_add_ps(lpc_supp_vec, lpc_vec_rev);

        _mm256_storeu_ps(lpc + j, lpc_vec);
        _mm256_storeu_ps(lpc + j_supp, lpc_supp_vec);
      }
      for (int j = (i/2) & ~0x7; j < i/2; j++) {
        int j_supp  = i - 1 - j;
        float tmp  = lpc[j];
        lpc[j]   += lambda * lpc[j_supp];
        lpc[j_supp] += lambda * tmp;
      }
#elif defined(__ARM_NEON__)
      const float32x4_t lambda_vec = vdupq_n_f32(lambda);
      for (int j = 0; j < i/2 - 3; j += 4) {
        int j_supp  = i - j - 4;
        float32x4_t lpc_vec = vld1q_f32(lpc + j);
        float32x4_t lpc_supp_vec = vld1q_f32(lpc + j_supp);

        float32x4_t lpc_supp_vec_rev = vrev64q_f32(lpc_supp_vec);
        lpc_supp_vec_rev = vcombine_f32(vget_high_f32(lpc_supp_vec_rev),
                                        vget_low_f32(lpc_supp_vec_rev));
        float32x4_t lpc_vec_rev = vrev64q_f32(lpc_vec);
        lpc_vec_rev = vcombine_f32(vget_high_f32(lpc_vec_rev),
                                   vget_low_f32(lpc_vec_rev));

        lpc_vec = vmlaq_f32(lpc_vec, lpc_supp_vec_rev, lambda_vec);
        lpc_supp_vec = vmlaq_f32(lpc_supp_vec, lpc_vec_rev, lambda_vec);

        vst1q_f32(lpc + j, lpc_vec);
        vst1q_f32(lpc + j_supp, lpc_supp_vec);
      }
      for (int j = (i/2) & ~0x3; j < i/2; j++) {
        int j_supp  = i - 1 - j;
        float tmp  = lpc[j];
        lpc[j]   += lambda * lpc[j_supp];
        lpc[j_supp] += lambda * tmp;
      }
#else

#endif
    }
    if (i & 1) {
      lpc[i/2] += lambda * lpc[i/2];
    }
    lpc[i] = lambda;

    error -= lambda * lambda * error;
  }
  return error;
}


