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

void convolute_simd(int simd,
                    const float *x, size_t xLength,
                    const float *h, size_t hLength,
                    float *result) {
  for (int n = 0; n < (int)xLength; n++) {
    float sum = 0.f;
    int end = n + 1;
    if (end > (int)hLength) {
      end = hLength;
    }
    if (simd) {
#ifdef __AVX__
      if (end < 8) {
        for (int m = 0; m < end; m++) {
          sum += h[m] * x[n - m];
        }
      } else {
         int simdEnd =  end & ~7;
        __m256 accum = _mm256_setzero_ps();
        for (int m = 0; m < simdEnd; m += 8) {
          __m256 xvec = _mm256_loadu_ps(x + n - m - 7);
          __m256 hvec = _mm256_loadu_ps(h + m);
          xvec = _mm256_permute2f128_ps(xvec, xvec, 1);
          xvec = _mm256_permute_ps(xvec, 27);
          __m256 mulres = _mm256_mul_ps(xvec, hvec);
          accum = _mm256_add_ps(accum, mulres);
        }
        accum = _mm256_hadd_ps(accum, accum);
        accum = _mm256_hadd_ps(accum, accum);
        sum = accum[0] + accum[4];
        for (int m = simdEnd; m < end; m++) {
          sum += h[m] * x[n - m];
        }
      }
    } else {
#elif defined(__ARM_NEON__)
      if (end < 4) {
        for (int m = 0; m < end; m++) {
          sum += h[m] * x[n - m];
        }
      } else {
         int simdEnd =  end & ~3;
        float32x4_t accum = vdupq_n_f32(0.f);
        for (int m = 0; m < simdEnd; m += 4) {
          float32x4_t xvec = vld1q_f32(x + n - m - 3);
          xvec = vrev64q_f32(xvec);
          float32x4_t hvec = vld1q_f32(h + m);
          accum = vmlaq_f32(xvec, hvec, accum);
        }
        float32x2_t accum2 = vpadd_f32(vget_high_f32(accum),
                                       vget_low_f32(accum));
        sum = vget_lane_f32(accum2, 0) + vget_lane_f32(accum2, 1);
        for (int m = simdEnd; m < end; m++) {
          sum += h[m] * x[n - m];
        }
      }
    } else {
#else
    } {
#endif
      for (int m = 0; m < end; m++) {
        sum += h[m] * x[n - m];
      }
    }
    result[n] = sum;
  }
}

void convolute_overlap_save(const float *__restrict x, size_t xLength,
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

void convolute_fft(const float *__restrict x, size_t xLength,
                   const float *__restrict h, size_t hLength,
                   float *result) {
  assert(hLength > 0);
  assert(xLength > 0);

  int M = xLength + hLength - 1;
  if ((M & (M - 1)) != 0) {
    int log = 1;
    while (M >>= 1) {
      log++;
    }
    M = (1 << log);
  }

  // Now M is the nearest greater than or equal power of 2.
  // Do zero padding of x and h
  // Allocate 2 extra samples for the M/2 complex number.
  float *X = mallocf(M + 2);
  memcpy(X, x, xLength * sizeof(x[0]));
  memsetf(X + xLength, M + 2 - xLength, 0.f);
  float *H = mallocf(M + 2);
  memcpy(H, h, hLength * sizeof(h[0]));
  memsetf(H + hLength, M + 2 - hLength, 0.f);

  // Prepare the forward FFT plan
  float *inputs[2] = { X, H };
  FFTFInstance *fftPlan = fftf_init_batch(
      FFTF_TYPE_REAL, FFTF_DIRECTION_FORWARD,
      FFTF_DIMENSION_1D, &M,
      FFTF_NO_OPTIONS, 2, (const float *const *) inputs,
      inputs);
  assert(fftPlan);
  // Prepare the inverse FFT plan
  FFTFInstance *fftInversePlan = fftf_init(
      FFTF_TYPE_REAL, FFTF_DIRECTION_BACKWARD,
      FFTF_DIMENSION_1D, &M,
      FFTF_NO_OPTIONS, X, X);
  assert(fftInversePlan);

  fftf_calc(fftPlan);

  int istart = 0;
#ifdef SIMD
  istart = M;
  for (int i = 0; i < M; i += FLOAT_STEP) {
    complex_multiply(X + i, H + i, X + i);
  }
#endif
  for (int i = istart; i < M + 2; i += 2) {
    complex_multiply_na(X + i, H + i, X + i);
  }

  // Return back from the Fourier representation
  fftf_calc(fftInversePlan);
  // Normalize
  real_multiply_scalar(X, xLength, 1.0f / M, result);

  // Release any resources allocated
  free(H);
  free(X);
  fftf_free(fftPlan);
  fftf_free(fftInversePlan);
}

void convolute(const float *__restrict x, size_t xLength,
               const float *__restrict h, size_t hLength,
               float *result) {
  // FIXME(v.markovtsev): conduct a complete benchmark and smartly choose the
  // FIXME(v.markovtsev): right approach.
  return convolute_simd(1, x, xLength, h, hLength, result);
}
