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

/// @brief Calculates the linear convolution of two signals using
/// the best method.
/// @param x The first signal (long one).
/// @param xLength The length of "x" array in float-s.
/// @param h The second signal (short one).
/// @param hLength The length of "h" array in float-s.
/// @param result The resulting signal of length xLength.
/// @note result and x may be the same arrays.
void convolute(const float *__restrict x, size_t xLength,
               const float *__restrict h, size_t hLength,
               float *result) NOTNULL(1, 3, 5);

/// @brief Calculates the linear convolution of two signals using
/// the FFT method.
/// @param x The first signal (long one).
/// @param xLength The length of "x" array in float-s.
/// @param h The second signal (short one).
/// @param hLength The length of "h" array in float-s.
/// @param result The resulting signal of length xLength.
/// @note result and x may be the same arrays.
void convolute_fft(const float *__restrict x, size_t xLength,
                   const float *__restrict h, size_t hLength,
                   float *result) NOTNULL(1, 3, 5);

/// @brief Calculates the linear convolution of two signals using
/// the overlap-save method.
/// @param x The first signal (long one).
/// @param xLength The length of "x" array in float-s.
/// @param h The second signal (short one).
/// @param hLength The length of "h" array in float-s.
/// @param result The resulting signal of length xLength.
/// @note result and x may be the same arrays.
void convolute_overlap_save(const float *__restrict x, size_t xLength,
                            const float *__restrict h, size_t hLength,
                            float *result) NOTNULL(1, 3, 5);

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

#ifdef __cplusplus
}
#endif

#endif  // SRC_PRIMITIVES_CONVOLUTE_H_
