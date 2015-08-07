/*! @file lpc.h
 *  @brief Linear Prediction Coefficients (LPC) calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 *
 *  @section License
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
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
