/*!
 *  @file lsp.c
 *  @brief Line Spectral Pair (LSP) functions.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 *  Copyright 2002-2006 Jean-Marc Valin
 *  Copyright 1993 David Rowe
 */

/*---------------------------------------------------------------------------*\
Original copyright
  FILE........: lsp.c
  AUTHOR......: David Rowe
  DATE CREATED: 24/2/93

  Introduction to Line Spectrum Pairs (LSPs)
  ------------------------------------------

  LSPs are used to encode the LPC filter coefficients {lpc} for
  transmission over the channel.  LSPs have several properties (like
  less sensitivity to quantisation noise) that make them superior to
  direct quantization of {lpc}.

  A(z) is a polynomial of order length with {lpc} as the coefficients.

  A(z) is transformed to P(z) and Q(z) (using a substitution and some
  algebra), to obtain something like:

    A(z) = 0.5[P(z)(z+z^-1) + Q(z)(z-z^-1)]  (1)

  As you can imagine A(z) has complex zeros all over the z-plane. P(z)
  and Q(z) have the very neat property of only having zeros _on_ the
  unit circle.  So to find them we take a test point z=exp(jw) and
  evaluate P (exp(jw)) and Q(exp(jw)) using a grid of points between 0
  and pi.

  The zeros (roots) of P(z) also happen to alternate, which is why we
  swap coefficients as we find roots.  So the process of finding the
  LSP frequencies is basically finding the roots of 5th order
  polynomials.

  The root so P(z) and Q(z) occur in symmetrical pairs at +/-w, hence
  the name Line Spectrum Pairs (LSPs).

  To convert back to lpc we just evaluate (1), "clocking" an impulse
  through it length times gives us the impulse response of A(z) which is
  {lpc}.

\*---------------------------------------------------------------------------*/

/*
   Heavily modified by Jean-Marc Valin (c) 2002-2006 (optimizations,
   additional functions, ...) and Vadim Markovtsev <v.markovtsev@samsung.com>
   (style, refactoring).

   This file contains functions for converting Linear Prediction
   Coefficients (LPC) to Line Spectral Pair (LSP) and back. Note that the
   LSP coefficients are not in radians format but in the x domain of the
   unit circle.

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
*/

#include "src/primitives/lsp.h"
#define _XOPEN_SOURCE
#include <math.h>
#include <simd/arithmetic-inl.h>
#include <simd/mathfun.h>

#define FREQ_SCALE 1.f
#define LPC_SCALING 1.f
#define LSP_SCALING 1.f

/// @brief Evaluates a series of Chebyshev polynomials.
/// @author David Rowe
/// @date 24/2/93
/// @note Only God and David Rowe know what the parameters mean.
static float cheb_poly_eval(const float *coef, float x, int m) {
   /* Initial conditions */
   float b0 = 0; /* b_(m+1) */
   float b1 = 0; /* b_(m+2) */
   x *= 2;

   /* Calculate the b_(k) */
   for (int k = m; k > 0; k--) {
      float tmp = b0;
      b0 = x * b0 - b1 + coef[m - k];
      b1 = tmp;
   }

   return -b1 + .5f * x * b0 + coef[m];
}

int lpc_to_lsp(int simd, const float *lpc, int length, int bisects, float delta,
               float *freq) {
  int roots = 0;             /* DR 8/2/94: number of roots found which will be
                                           returned   */
  int m = length / 2;              /* order of P'(z) & Q'(z) polynomials   */

  /* Allocate memory space for polynomials */
  float Q[m + 1];
  float P[m + 1];

  /* determine P'(z)'s and Q'(z)'s coefficients where
    P'(z) = P(z)/(1 + z^(-1)) and Q'(z) = Q(z)/(1-z^(-1)) */

  float *px = P;               /* ptrs of respective P'(z) & Q'(z)  */
  float *qx = Q;
  float *p = P;
  float *q = Q;

  *px++ = LPC_SCALING;
  *qx++ = LPC_SCALING;
  for (int i = 0; i < m; i++) {
    *px++ = lpc[i] + lpc[length - 1 - i] - *p++;
    *qx++ = lpc[i] - lpc[length - 1 - i] + *q++;
  }

  if (simd) {
    real_multiply_scalar(P, m, 2.f, P);
    real_multiply_scalar(Q, m, 2.f, Q);
  } else {
    real_multiply_scalar_na(P, m, 2.f, P);
    real_multiply_scalar_na(Q, m, 2.f, Q);
  }

  /* Search for a zero in P'(z) polynomial first and then alternate to Q'(z).
  Keep alternating between the two polynomials as each zero is found   */

  float xr = 0;               /* initialize xr to zero     */
  float xl = FREQ_SCALE;      /* start at point xl = 1     */

  for (int j = 0; j < length; j++) {
    /* determines whether P' or Q' is evaluated. */
    float *pt = (j & 1)? Q : P;  /* ptr used for cheb_poly_eval()
                                    either P' or Q' */
    float psuml = cheb_poly_eval(pt, xl, m);  /* evaluates poly at xl   */
    int flag = 1;  /* program is searching for a root is 1 else has found one */
    while (flag && xr >= -FREQ_SCALE) {
      /* Modified by JMV to provide smaller steps around x=+-1 */
      float dd = delta * (1 - 0.9f * xl * xl);
      if (fabs(psuml) < 0.2f) {
        dd *= 0.5f;
      }
      xr = xl - dd;                /* interval spacing   */
      float psumr = cheb_poly_eval(pt, xr, m);  /* poly(xl-delta_x)   */
      float temp_psumr = psumr;
      float temp_xr = xr;

      /*
      If no sign change increment xr and re-evaluate poly(xr). Repeat untill
      the sign changes.
      If a sign change has occurred the interval is bisected and then
      checked again for a sign change which determines in which
      interval the zero lies in.
      If there is no sign change between poly(xm) and poly(xl) set interval
      between xm and xr else set interval between xl and xr and repeat untill
      a root is located within the specified limits.
      */

      if (psumr * psuml <= 0) {
        roots++;
        float psumm = psuml;
        float xm;
        for (int k = 0; k <= bisects; k++) {
          xm = (xl + xr) / 2;          /* bisect the interval   */
          psumm = cheb_poly_eval(pt, xm, m);
          if (psumm * psuml > 0) {
            psuml = psumm;
            xl = xm;
          } else {
            psumr = psumm;
            xr = xm;
          }
        }

        /* once zero is found, reset initial interval to xr   */
        freq[j] = acosf(xm);
        xl = xm;
        flag = 0;           /* reset flag for next search   */
      } else {
        psuml = temp_psumr;
        xl = temp_xr;
      }
    }
  }
  return roots;
}

void lsp_to_lpc(int simd, const float *freq, int length, float *lpc) {
  int m = length / 2;
  float Wp[4 * m + 2];
  float *pw = Wp;

  /* set contents of buffer to 0 */
  memsetf(Wp, 0.f, 4 * m + 2);

  float x_freq[length];
  cos_psv(simd, freq, length, x_freq);

  /* reconstruct P(z) and Q(z) by  cascading second order
    polynomials in form 1 - 2xz(-1) +z(-2), where x is the
    LSP coefficient */
  for (int j = 0; j <= length; j++) {
    int i2 = 0;
    float *n4;
    float xout1, xout2, xin1, xin2;
    xin1 = 1.0;
    xin2 = 1.0;
    for (int i = 0; i < m; i++, i2 += 2) {
      float *n1 = pw + i * 4;
      float *n2 = n1 + 1;
      float *n3 = n2 + 1;
      n4 = n3 + 1;
      xout1 = xin1 - 2.f * x_freq[i2] * *n1 + *n2;
      xout2 = xin2 - 2.f * x_freq[i2 + 1] * *n3 + *n4;
      *n2 = *n1;
      *n4 = *n3;
      *n1 = xin1;
      *n3 = xin2;
      xin1 = xout1;
      xin2 = xout2;
    }
    xout1 = xin1 + *(n4+1);
    xout2 = xin2 - *(n4+2);
    if (j > 0) {
      lpc[j-1] = (xout1 + xout2) / 2;
    }
    *(n4 + 1) = xin1;
    *(n4 + 2) = xin2;

    xin1 = 0.0;
    xin2 = 0.0;
  }
}

void lsp_enforce_margin(int length, float margin, float *lsp) {
  if (lsp[0] < LSP_SCALING * margin) {
    lsp[0] = LSP_SCALING * margin;
  }
  if (lsp[length - 1] > LSP_SCALING * (M_PI - margin)) {
    lsp[length - 1] = LSP_SCALING * (M_PI - margin);
  }
  for (int i = 1; i < length - 1; i++) {
    if (lsp[i] < lsp[i - 1] + LSP_SCALING * margin) {
      lsp[i] = lsp[i - 1] + LSP_SCALING * margin;
    }
    if (lsp[i] > lsp[i + 1] - LSP_SCALING * margin) {
      lsp[i]= .5f * (lsp[i] + lsp[i + 1] - LSP_SCALING * margin);
    }
  }
}
