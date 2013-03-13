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

#include <spfextr/config.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Performs an atomic wavelet transform on series of real numbers.
/// @param src An array of floating point numbers to transform.
/// @param length The logical length of src (in float-s, not in bytes).
/// @param order The order of Daubechies wavelet to apply divided by 2.
/// For example, order = 3 means 6 coefficients.
/// @param desthi The high frequency part of result (highpass).
/// @param destlo The low frequency part of result (lowpass).
/// @details Daubechies wavelet of order 8 is used.
void wavelet_apply(const float *__restrict src, size_t length,
                   float *__restrict desthi, float *__restrict destlo)
    NOTNULL(1, 3, 4);

#ifdef __cplusplus
}
#endif

#endif  // SRC_PRIMITIVES_WAVELET_H_
