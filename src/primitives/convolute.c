/*! @file convolute.c
 *  @brief Calculates the linear convolution of two signals.
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

void convolute_ones(const float *__restrict x, size_t xLength,
                    int k, int count, float *result) {
  for (int i = 0; i < (int)xLength; ++i) {
    result[i] = x[i] + (i >= k ? result[i - k] : 0) - (i >= k*count ? x[i - k * count] : 0);
  }
}

ConvoluteOverlapSaveHandle convolute_overlap_save_initialize(
    size_t xLength, size_t hLength) {
  assert(hLength < xLength / 2);
  assert(xLength > 0);
  assert(hLength > 0);

  ConvoluteOverlapSaveHandle handle;
  size_t M = hLength;  //  usual designation
  handle.x_length = xLength;
  handle.h_length = hLength;

  // Do zero padding of h to the next power of 2 + extra 2 float-s
  size_t L = M;
  int log = 2;
  while (L >>= 1) {
    log++;
  }
  L = (1 << log);
  handle.H = mallocf(L + 2);
  assert(handle.H);
  memsetf(handle.H + M, L - M, 0.f);
  handle.L = malloc(sizeof(L));
  *handle.L = L;

  handle.fft_boiler_plate = mallocf(L + 2);
  assert(handle.fft_boiler_plate);

  handle.fft_plan = fftf_init(FFTF_TYPE_REAL, FFTF_DIRECTION_FORWARD,
                              FFTF_DIMENSION_1D, handle.L,
                              FFTF_NO_OPTIONS, handle.fft_boiler_plate,
                              handle.fft_boiler_plate);

  assert(handle.fft_plan);

  handle.fft_inverse_plan = fftf_init(
      FFTF_TYPE_REAL, FFTF_DIRECTION_BACKWARD,
      FFTF_DIMENSION_1D, handle.L,
      FFTF_NO_OPTIONS, handle.fft_boiler_plate,
      handle.fft_boiler_plate);
  assert(handle.fft_inverse_plan);

  return handle;
}

void convolute_overlap_save_finalize(ConvoluteOverlapSaveHandle handle) {
  free(handle.fft_boiler_plate);
  fftf_destroy(handle.fft_plan);
  fftf_destroy(handle.fft_inverse_plan);
  free(handle.L);
  free(handle.H);
}

void convolute_overlap_save(ConvoluteOverlapSaveHandle handle,
                            const float *__restrict x,
                            const float *__restrict h,
                            float *result) {
  assert(x != NULL);
  assert(h != NULL);
  assert(result != NULL);

  size_t M = handle.h_length;  //  usual designation
  int L = *handle.L;
  // Do zero padding of h to the next power of 2 + extra 2 float-s
  memcpy(handle.H, h, handle.h_length * sizeof(float));

  // H = FFT(paddedH, L)
  size_t fftComplexSize = (L + 2) * sizeof(float);
  memcpy(handle.fft_boiler_plate, handle.H, L * sizeof(float));

  fftf_calc(handle.fft_plan);
  memcpy(handle.H, handle.fft_boiler_plate, fftComplexSize);

  int step = L - (M - 1);
  // Note: no "#pragma omp parallel for" here since
  // handle.fft_boiler_plate is shared AND FFTF should utilize all available resources.
  for (size_t i = 0; i < handle.x_length; i += step) {
    // X = [zeros(1, M - 1), x, zeros(1, L-1)];
    // we must run FFT on X[i, i + L].
    // No X is really needed, some index arithmetic is used.
    if (i > 0) {
      if (i + step <= handle.x_length) {
        memcpy(handle.fft_boiler_plate, x + i - (M - 1), L * sizeof(float));
      } else {
        int cl = handle.x_length - i + M - 1;
        memcpy(handle.fft_boiler_plate, x + i - (M - 1),
               cl * sizeof(float));
        memsetf(handle.fft_boiler_plate + cl, L - cl, 0.f);
      }
    } else {
      memsetf(handle.fft_boiler_plate, M - 1, 0.f);
      memcpy(handle.fft_boiler_plate + M - 1, x, step * sizeof(float));
    }
    fftf_calc(handle.fft_plan);

    // fftBoilerPlate = fftBoilerPlate * H (complex arithmetic)
    int cciStart = 0;
#ifdef SIMD
    cciStart = L;
    for (int cci = 0; cci < L; cci += FLOAT_STEP) {
      complex_multiply(handle.fft_boiler_plate + cci, handle.H + cci,
                       handle.fft_boiler_plate + cci);
    }
#endif
    for (int cci = cciStart; cci < L + 2; cci += 2) {
      complex_multiply_na(handle.fft_boiler_plate + cci, handle.H + cci,
                          handle.fft_boiler_plate + cci);
    }

    // Return back from the Fourier representation
    fftf_calc(handle.fft_inverse_plan);
    // Normalize
    real_multiply_scalar(handle.fft_boiler_plate + M - 1, step, 1.0f / L,
                         handle.fft_boiler_plate + M - 1);

    if (i + step <= handle.x_length) {
      memcpy(result + i, handle.fft_boiler_plate + M - 1,
             step * sizeof(float));
    } else {
      memcpy(result + i, handle.fft_boiler_plate + M - 1,
             (handle.x_length - i) * sizeof(float));
    }
  }
}

ConvoluteFFTHandle convolute_fft_initialize(size_t xLength, size_t hLength) {
  assert(hLength > 0);
  assert(xLength > 0);

  ConvoluteFFTHandle handle;

  int M = xLength + hLength - 1;
  if ((M & (M - 1)) != 0) {
    int log = 1;
    while (M >>= 1) {
      log++;
    }
    M = (1 << log);
  }
  handle.M = malloc(sizeof(M));
  *handle.M = M;
  handle.x_length = xLength;
  handle.h_length = hLength;

  // Now M is the nearest greater than or equal power of 2.
  // Do zero padding of x and h
  // Allocate 2 extra samples for the M/2 complex number.
  float *X = mallocf(M + 2);
  memsetf(X + xLength, M + 2 - xLength, 0.f);
  float *H = mallocf(M + 2);
  memsetf(H + hLength, M + 2 - hLength, 0.f);

  handle.inputs = malloc(2 * sizeof(float *));
  handle.inputs[0] = X;
  handle.inputs[1] = H;

  // Prepare the forward FFT plan
  handle.fft_plan = fftf_init_batch(
      FFTF_TYPE_REAL, FFTF_DIRECTION_FORWARD,
      FFTF_DIMENSION_1D, handle.M,
      FFTF_NO_OPTIONS, 2, (const float *const *)handle.inputs,
      handle.inputs);
  assert(handle.fft_plan);

  // Prepare the inverse FFT plan
  handle.fft_inverse_plan = fftf_init(
    FFTF_TYPE_REAL, FFTF_DIRECTION_BACKWARD,
    FFTF_DIMENSION_1D, handle.M,
    FFTF_NO_OPTIONS, X, X);
  assert(handle.fft_inverse_plan);
  return handle;
}

void convolute_fft_finalize(ConvoluteFFTHandle handle) {
  free(handle.inputs[0]);
  free(handle.inputs[1]);
  free(handle.inputs);
  free(handle.M);
  fftf_destroy(handle.fft_plan);
  fftf_destroy(handle.fft_inverse_plan);
}

void convolute_fft(ConvoluteFFTHandle handle,
                   const float *x, const float *h,
                   float *result) {
  assert(x != NULL);
  assert(h != NULL);
  assert(result != NULL);

  float *X = handle.inputs[0];
  float *H = handle.inputs[1];
  int xLength = handle.x_length;
  int hLength = handle.h_length;
  int M = *handle.M;
  memcpy(X, x, xLength * sizeof(x[0]));
  memcpy(H, h, hLength * sizeof(h[0]));

  // fft(X), fft(H)
  fftf_calc(handle.fft_plan);

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
  fftf_calc(handle.fft_inverse_plan);
  // Normalize
  real_multiply_scalar(X, xLength, 1.0f / M, result);
}

ConvoluteHandle convolute_initialize(size_t xLength, size_t hLength) {
  ConvoluteHandle handle;
  handle.x_length = xLength;
  handle.h_length = hLength;
#ifdef __ARM_NEON__
  if (xLength > hLength * 2) {
    if (xLength > 200) {
      handle.algorithm = kConvolutionAlgorithmOverlapSave;
      handle.handle.os = convolute_overlap_save_initialize(xLength, hLength);
    } else {
      handle.algorithm = kConvolutionAlgorithmBruteForce;
    }
  } else {
    if (xLength > 300) {
      handle.algorithm = kConvolutionAlgorithmFFT;
      handle.handle.fft = convolute_fft_initialize(xLength, hLength);
    } else {
      handle.algorithm = kConvolutionAlgorithmBruteForce;
    }
  }
#else
  if (xLength > hLength * 2) {
    if (xLength > 200) {
      handle.algorithm = kConvolutionAlgorithmOverlapSave;
      handle.handle.os = convolute_overlap_save_initialize(xLength, hLength);
    } else {
      handle.algorithm = kConvolutionAlgorithmBruteForce;
    }
  } else {
    if (xLength > 350) {
      handle.algorithm = kConvolutionAlgorithmFFT;
      handle.handle.fft = convolute_fft_initialize(xLength, hLength);
    } else {
      handle.algorithm = kConvolutionAlgorithmBruteForce;
    }
  }
#endif
  return handle;
}

void convolute_finalize(ConvoluteHandle handle) {
  switch (handle.algorithm) {
    case kConvolutionAlgorithmFFT:
      convolute_fft_finalize(handle.handle.fft);
      break;
    case kConvolutionAlgorithmOverlapSave:
      convolute_overlap_save_finalize(handle.handle.os);
      break;
    case kConvolutionAlgorithmBruteForce:
      break;
  }
}

void convolute(ConvoluteHandle handle,
               const float *x, const float *h,
               float *result) {
  switch (handle.algorithm) {
    case kConvolutionAlgorithmFFT:
      convolute_fft(handle.handle.fft, x, h, result);
      break;
    case kConvolutionAlgorithmOverlapSave:
      convolute_overlap_save(handle.handle.os, x, h, result);
      break;
    case kConvolutionAlgorithmBruteForce:
      convolute_simd(1, x, handle.x_length, h, handle.h_length, result);
      break;
  }
}
