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
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include "src/primitives/memory.h"
#include "src/primitives/arithmetic.h"

void convolute(const float *x, size_t xLength, const float *h, size_t hLength,
               float *result) {
  assert(hLength <= xLength);
  assert(xLength > 0);
  assert(hLength > 0);

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

  int step = L - (hLength - 1);
  for (size_t i = 0; i < xLength; i += step) {
    if (i > xLength - step) {
      i = xLength - step;
    }
    memcpy(fftBoilerPlate, x + i, L * sizeof(float));
    fftf_calc(fftPlan);
    // fftBoilerPlate = fftBoilerPlate * H (complex arithmetic)
    int cciStart = 0;
#ifdef __AVX__
    cciStart = L;
    for (int cci = 0; cci < (int)L; cci += 8) {
      complex_multiply(fftBoilerPlate + cci, H + cci, fftBoilerPlate + cci);
    }
#elif defined(__ARM_NEON__)
    cciStart = L;
    for (int cci = 0; cci < L; cci += 4) {
      complex_multiply(fftBoilerPlate + cci, H + cci, fftBoilerPlate + cci);
    }
#endif
    for (int cci = cciStart; cci < (int)L + 2; cci += 2) {
      complex_multiply_na(fftBoilerPlate + cci, H + cci, fftBoilerPlate + cci);
    }
    fftf_calc(fftInversePlan);
    memcpy(result + i, fftBoilerPlate + hLength - 1, step * sizeof(float));
  }

  free(H);
  free(fftBoilerPlate);
  fftf_free(fftPlan);
  fftf_free(fftInversePlan);
}
