/*! @file energy.h
 *  @brief Low level energy calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_PRIMITIVES_ENERGY_H_
#define SRC_PRIMITIVES_ENERGY_H_

#include <stddef.h>
#include "src/config.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Squares each value of signal and sums the results.
/// @param simd Value indicating whether to use SIMD acceleration.
/// @param signal The source array of floating point numbers.
/// length The number of items in the array.
float calculate_energy(int simd, const float *signal, size_t length) NOTNULL(2);

#ifdef __cplusplus
}
#endif

#endif  // SRC_PRIMITIVES_ENERGY_H_
