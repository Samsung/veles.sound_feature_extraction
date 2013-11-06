/*
  Copyright 1992, 1993, 1994 by Jutta Degener and Carsten Bormann,
  Technische Universitaet Berlin

  Any use of this software is permitted provided that this notice is not
  removed and that neither the authors nor the Technische Universitaet Berlin
  are deemed to have made any representations as to the suitability of this
  software for any purpose nor are held responsible for any defects of
  this software.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.

  As a matter of courtesy, the authors request to be informed about uses
  this software has found, about bugs in this software, and about any
  improvements that may be of general interest.

  Berlin, 28.11.1994
  Jutta Degener
  Carsten Bormann

  Since the original terms of 15 years ago maybe do not make our
  intentions completely clear given today's refined usage of the legal
  terms, we append this additional permission:

        Permission to use, copy, modify, and distribute this software
        for any purpose with or without fee is hereby granted,
        provided that this notice is not removed and that neither
        the authors nor the Technische Universitaet Berlin are
        deemed to have made any representations as to the suitability
        of this software for any purpose nor are held responsible
        for any defects of this software.  THERE IS ABSOLUTELY NO
        WARRANTY FOR THIS SOFTWARE.

  Berkeley/Bremen, 05.04.2009
  Jutta Degener
  Carsten Bormann


   Code modified by Jean-Marc Valin

   Speex License:

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    Modified by Vadim Markovtsev <v.markovtsev@samsung.com>
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
   for (int i = 0; i < length; i++) {
      /* Sum up this iteration's reflection coefficient */
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

      float r = rr / error;
      /*  Update LPC coefficients and total error */
      lpc[i] = r;
      for (int j = 0; j < i/2; j++) {
         float tmp  = lpc[j];
         int ind  = i - 1 - j;
         lpc[j]   = lpc[j] + r * lpc[ind];
         lpc[ind] = lpc[ind] + r * tmp;
      }
      if (i & 1) {
        lpc[i/2] = lpc[i/2] + lpc[i/2] * r;
      }

      error -= error * r * r;
   }
   return error;
}


