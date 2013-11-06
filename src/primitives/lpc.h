/*! @file lpc.h
 *  @brief Linear Prediction Coefficients (LPC) calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_PRIMITIVES_LPC_H_
#define SRC_PRIMITIVES_LPC_H_

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Calculates Linear Prediction Coefficients (LPC) using
/// Levinson-Durbin recursion.
/// @param simd Value indicating whether to use SIMD acceleration.
/// @param ac [0...length) autocorrelation values.
/// @param length The size of ac (in float-s, not in bytes).
/// @param lpc The resulting LPC [0...length - 1).
/// @return Minimum mean square error of the coefficients estimation.
/// @details Method was invented by N. Levinson in 1947, modified by
/// J. Durbin in 1959.
float ldr_lpc(int simd,  const float *ac, int length, float *lpc);

#ifdef __cplusplus
}
#endif

#endif  // SRC_PRIMITIVES_LPC_H_
