/*! @file energy.h
 *  @brief Low level energy calculation.
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

#ifndef SRC_PRIMITIVES_ENERGY_H_
#define SRC_PRIMITIVES_ENERGY_H_

#include <stddef.h>
#include "src/config.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Squares each value of signal and sums the results.
/// @param simd Value indicating whether to use SIMD acceleration.
/// @param norm Divide the result by length.
/// @param signal The source array of floating point numbers.
/// length The number of items in the array.
float calculate_energy(int simd, int norm, const float *signal,
                       size_t length) NOTNULL(3);

#ifdef __cplusplus
}
#endif

#endif  // SRC_PRIMITIVES_ENERGY_H_
