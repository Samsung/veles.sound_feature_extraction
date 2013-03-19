/*! @file wavelet.h
 *  @brief Wavelet low level functions.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_PRIMITIVES_WAVELET_H_
#define SRC_PRIMITIVES_WAVELET_H_

#include "src/config.h"
#include <stddef.h>
#include "src/primitives/wavelet_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Checks the specified wavelet order for validity.
/// @param type The wavelet type.
/// @param order The order of the wavelet.
/// @return 1 if order is valid, otherwise 0.
int wavelet_validate_order(WaveletType type, int order);

/// @brief If AVX SIMD instruction set is used, copies the source signal to
/// a new array with specific excessive aligned format, else just returns src.
/// @param order The number of applied wavelet's coefficients.
/// @param src The source signal.
/// @param length The length of source in float-s.
/// @return A newly allocated memory block which should be disposed with
/// free() if AVX SIMD is activated; otherwise, src.
float *wavelet_prepare_array(int order, const float *src, size_t length)
    NOTNULL(2)
#ifdef __AVX__
    MALLOC WARN_UNUSED_RESULT
#endif
;

/// @brief Allocates the array which is capable of storing one of the two
/// wavelet splitted parts of a signal (that is, desthi or destlo in
/// wavelet_apply()).
/// @param order The number of applied wavelet's coefficients.
/// @param sourceLength The logical length of the source array
/// (in float-s, not in bytes).
/// @return A newly allocated memory block which should be disposed with
/// free().
float *wavelet_allocate_destination(int order, size_t sourceLength)
    MALLOC WARN_UNUSED_RESULT;

/// @brief Splits src into four regions to reuse the allocated memory.
void wavelet_recycle_source(int order, float *src, size_t length,
                            float **desthihi, float **desthilo,
                            float **destlohi, float **destlolo)
    NOTNULL(2, 4, 5, 6, 7);

/// @brief Performs an atomic wavelet transform on series of real numbers.
/// @param type The wavelet type.
/// @param order The order of the wavelet to apply.
/// For example, order = 6 means 6 coefficients.
/// @param src An array of floating point numbers to transform.
/// @param length The logical length of src (in float-s, not in bytes).
/// @param desthi The high frequency part of result (highpass).
/// @param destlo The low frequency part of result (lowpass).
/// @details Daubechies wavelet of order 8 is used.
/// @pre length must be greater than or equal to 8.
/// @pre length must be even.
void wavelet_apply(WaveletType type, int order,
                   const float *__restrict src, size_t length,
                   float *__restrict desthi, float *__restrict destlo)
    NOTNULL(3, 5, 6);

/// @brief Performs an atomic wavelet transform on series of real numbers.
/// (no SIMD acceleration is used).
/// @param type The wavelet type.
/// @param order The order of the wavelet to apply.
/// For example, order = 6 means 6 coefficients.
/// @param src An array of floating point numbers to transform.
/// @param length The logical length of src (in float-s, not in bytes).
/// @param desthi The high frequency part of result (highpass).
/// @param destlo The low frequency part of result (lowpass).
/// @pre length must be greater than or equal to (order * 2).
/// @pre length must be even.
void wavelet_apply_na(WaveletType type, int order,
                      const float *__restrict src, size_t length,
                      float *__restrict desthi, float *__restrict destlo)
    NOTNULL(3, 5, 6);

#ifdef __cplusplus
}
#endif

#endif  // SRC_PRIMITIVES_WAVELET_H_
