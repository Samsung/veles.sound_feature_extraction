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
 */

#include "src/primitives/lpc.h"
#include <assert.h>
#include <math.h>
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

float ldr_lpc(int simd, const float *ac, int length, float *lpc) {
   if (ac[0] == 0) {
      for (int i = 0; i < length; i++) {
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
          i < 16
#elif defined(__ARM_NEON__)
          i < 8
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
        rr -= accum[0] + accum[4];
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
#error Not supported
#endif
      }
      float lambda = rr / error;

      /*  Update LPC coefficients and total error */
      for (int j = 0; j < i/2; j++) {
         int j_supp  = i - 1 - j;
         float tmp  = lpc[j];
         lpc[j]   += lambda * lpc[j_supp];
         lpc[j_supp] += lambda * tmp;
      }
      if (i & 1) {
        lpc[i/2] += lambda * lpc[i/2];
      }
      lpc[i] = lambda;

      error -= lambda * lambda * error;
   }
   return error;
}


