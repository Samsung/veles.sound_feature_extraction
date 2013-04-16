/*! @file convolute.c
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/primitives/convolute.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <fftf/api.h>
#include "src/primitives/arithmetic-inl.h"

/*
/// @brief Brute-force calculation method used for debugging FFT one.
static void convolute_circular(const float *__restrict x,
                               const float *__restrict h,
                               size_t N,
                               float *__restrict result) {
  for (int n = 0; n < (int)N; n++) {
    float sum = 0.f;
    for (int m = 0; m <= n; m++) {
      sum += x[m] * h[n - m];
    }
    for (int m = n + 1; m < (int)N; m++) {
      sum += x[m] * h[N - m + n];
    }
    result[n] = sum;
  }
}
*/

void convolute(const float *__restrict x, size_t xLength,
               const float *__restrict h, size_t hLength,
               float *result) {
  assert(hLength < xLength / 2);
  assert(xLength > 0);
  assert(hLength > 0);
  size_t M = hLength;  //  usual designation

  // Using overlap-save algorithm here

  // Do zero padding of h to the next power of 2 + extra 2 float-s
  size_t L;
  float *H = zeropaddingex(h, hLength, &L, 2);
  assert(H);

  // H = FFT(paddedH, L)
  size_t fftComplexSize = (L + 2) * sizeof(float);
  float *fftBoilerPlate = mallocf(L + 2);
  assert(fftBoilerPlate);
  memcpy(fftBoilerPlate, H, L * sizeof(float));
  int fftLength = L;
  FFTFInstance *fftPlan = fftf_init(FFTF_TYPE_REAL, FFTF_DIRECTION_FORWARD,
                                    FFTF_DIMENSION_1D, &fftLength,
                                    FFTF_NO_OPTIONS, fftBoilerPlate,
                                    fftBoilerPlate);
  assert(fftPlan);
  fftf_calc(fftPlan);
  memcpy(H, fftBoilerPlate, fftComplexSize);

  // Prepare the inverse FFT plan
  FFTFInstance *fftInversePlan = fftf_init(
      FFTF_TYPE_REAL, FFTF_DIRECTION_BACKWARD,
      FFTF_DIMENSION_1D, &fftLength,
      FFTF_NO_OPTIONS, fftBoilerPlate,
      fftBoilerPlate);
  assert(fftInversePlan);

  int step = L - (M - 1);
  // Note: no "#pragma omp parallel for" here since
  // fftBoilerPlate is shared AND FFTF should utilize all available resources.
  for (size_t i = 0; i < xLength; i += step) {
    // X = [zeros(1, M - 1), x, zeros(1, L-1)];
    // we must run FFT on X[i, i + L].
    // No X is really needed, some index arithmetic is used.
    if (i > 0) {
      if (i + step <= xLength) {
        memcpy(fftBoilerPlate, x + i - (M - 1), L * sizeof(float));
      } else {
        int cl = xLength - i + M - 1;
        memcpy(fftBoilerPlate, x + i - (M - 1),
               cl * sizeof(float));
        memsetf(fftBoilerPlate + cl, L - cl, 0.f);
      }
    } else {
      memsetf(fftBoilerPlate, M - 1, 0.f);
      memcpy(fftBoilerPlate + M - 1, x, step * sizeof(float));
    }
    fftf_calc(fftPlan);

    // fftBoilerPlate = fftBoilerPlate * H (complex arithmetic)
    int cciStart = 0;
#ifdef SIMD
    cciStart = L;
    for (int cci = 0; cci < (int)L; cci += FLOAT_STEP) {
      complex_multiply(fftBoilerPlate + cci, H + cci, fftBoilerPlate + cci);
    }
#endif
    for (int cci = cciStart; cci < (int)L + 2; cci += 2) {
      complex_multiply_na(fftBoilerPlate + cci, H + cci, fftBoilerPlate + cci);
    }

    // Return back from the Fourier representation
    fftf_calc(fftInversePlan);
    // Normalize
    real_multiply_scalar(fftBoilerPlate + M - 1, step, 1.0f / L, fftBoilerPlate + M - 1);

    if (i + step <= xLength) {
      memcpy(result + i, fftBoilerPlate + M - 1, step * sizeof(float));
    } else {
      memcpy(result + i, fftBoilerPlate + M - 1, (xLength - i) * sizeof(float));
    }
  }

  // Release any resources allocated
  free(H);
  free(fftBoilerPlate);
  fftf_free(fftPlan);
  fftf_free(fftInversePlan);
}
