/*! @file convolute.h
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

#ifndef SRC_PRIMITIVES_CONVOLUTE_H_
#define SRC_PRIMITIVES_CONVOLUTE_H_

#include <stddef.h>
#include "src/config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void *fft_plan;
  void *fft_inverse_plan;
  int *M;
  int x_length;
  int h_length;
  float **inputs;
} ConvoluteFFTHandle;

/// @brief Prepares for the calculation of linear convolution of two signals
/// using the FFT method.
/// @param xLength The length of "x" array in float-s.
/// @param hLength The length of "h" array in float-s.
/// @return The handle for convolute_fft().
ConvoluteFFTHandle convolute_fft_prepare(size_t xLength, size_t hLength);

/// @brief Calculates the linear convolution of two signals using
/// the FFT method.
/// @param handle The structure obtained from convolute_fft_prepare().
/// @param x The first signal (long one).
/// @param h The second signal (short one).
/// @param result The resulting signal of length xLength.
/// @note result and x may be the same arrays.
void convolute_fft(ConvoluteFFTHandle handle,
                   const float *x, const float *h,
                   float *result) NOTNULL(2, 3, 4);

/// @brief Frees any resources allocated by convolute_fft_prepare().
/// @param handle The structure obtained from convolute_fft_prepare().
void convolute_fft_finalize(ConvoluteFFTHandle handle);

typedef struct {
  void *fft_plan;
  void *fft_inverse_plan;
  float *fft_boiler_plate;
  float *H;
  size_t x_length;
  size_t h_length;
  int *L;
} ConvoluteOverlapSaveHandle;

/// @brief Prepares for the calculation of linear convolution of two signals
/// using the overlap-save method.
/// @param xLength The length of "x" array in float-s.
/// @param hLength The length of "h" array in float-s.
/// @return The handle for convolute_overlap_save().
ConvoluteOverlapSaveHandle convolute_overlap_save_prepare(
    size_t xLength, size_t hLength);

/// @brief Calculates the linear convolution of two signals using
/// the overlap-save method.
/// @param handle The structure obtained from convolute_overlap_save_prepare().
/// @param x The first signal (long one).
/// @param h The second signal (short one).
/// @param result The resulting signal of length xLength.
/// @note result and x may be the same arrays.
void convolute_overlap_save(ConvoluteOverlapSaveHandle handle,
                            const float *__restrict x,
                            const float *__restrict h,
                            float *result) NOTNULL(2, 3, 4);

/// @brief Frees any resources allocated by convolute_overlap_save_prepare().
/// @param handle The structure obtained from convolute_overlap_save_prepare().
void convolute_overlap_save_finalize(ConvoluteOverlapSaveHandle handle);

/// @brief Calculates the linear convolution of two signals using
/// the "brute force" method.
/// @param simd Value indicating whether to use SIMD acceleration or not.
/// @param x The first signal (long one).
/// @param xLength The length of "x" array in float-s.
/// @param h The second signal (short one).
/// @param hLength The length of "h" array in float-s.
/// @param result The resulting signal of length xLength.
/// @note result and x may be the same arrays.
void convolute_simd(int simd,
                    const float *x, size_t xLength,
                    const float *h, size_t hLength,
                    float *result) NOTNULL(2, 4, 6);

/// @brief Calculates the linear convolution of two signals, the second
/// signal being the periodic series of 1 and 0.
/// @param x The first signal (long one).
/// @param xLength The length of "x" array in float-s.
/// @param k The length of the second signal's period.
/// @note result and x may be the same arrays.
/// @details Convolution for periodical filter which looks like:
/// {1 {(k - 1) zeroes} 1 {(k - 1) zeroes} ..}.
void convolute_ones(const float *__restrict x, size_t xLength,
                    int k, float *result);

typedef enum {
  kConvolutionAlgorithmBruteForce,
  kConvolutionAlgorithmFFT,
  kConvolutionAlgorithmOverlapSave
} ConvolutionAlgorithm;

typedef struct {
  ConvolutionAlgorithm algorithm;
  int x_length;
  int h_length;
  union {
    ConvoluteFFTHandle fft;
    ConvoluteOverlapSaveHandle os;
  } handle;
} ConvoluteHandle;

/// @brief Prepares for the calculation of linear convolution of two signals
/// using the best method.
/// @param xLength The length of "x" array in float-s.
/// @param hLength The length of "h" array in float-s.
/// @return The handle for convolute().
ConvoluteHandle convolute_prepare(size_t xLength, size_t hLength);

/// @brief Calculates the linear convolution of two signals using
/// the best method.
/// @param handle The structure obtained from convolute_prepare().
/// @param x The first signal (long one).
/// @param h The second signal (short one).
/// @param result The resulting signal of length xLength.
/// @note result and x may be the same arrays.
void convolute(ConvoluteHandle handle,
               const float *x, const float *h,
               float *result) NOTNULL(2, 3, 4);

/// @brief Frees any resources allocated by convolute_overlap_prepare().
/// @param handle The structure obtained from convolute_overlap_prepare().
void convolute_finalize(ConvoluteHandle handle);

#ifdef __cplusplus
}
#endif

#endif  // SRC_PRIMITIVES_CONVOLUTE_H_
