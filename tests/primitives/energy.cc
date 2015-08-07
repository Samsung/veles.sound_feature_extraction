/*! @file energy.cc
 *  @brief Tests for src/primitives/energy.cc.
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


#include <gtest/gtest.h>
#include "src/primitives/energy.h"
#include <simd/arithmetic-inl.h>

TEST(Energy, calculate_energy) {
  const int length = 510;
#ifdef __AVX__
#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
  alignas(std::alignment_of<__m256>::value) float array[length];
#else
  __attribute__((aligned(64))) float array[length];
#endif
#else
  __attribute__((aligned(32))) float array[length];
#endif
  for (int i = 0; i < length; i++) {
    array[i] = i + 1;
  }
  float result = calculate_energy(true, true, array, length);
  ASSERT_NEAR((length + 1) * (2 * length + 1.0f) / 6, result, 0.01f);
}

#define TEST_NAME Energy
#define ITER_COUNT 500000
#define CUSTOM_FUNC_BASELINE(input, length) calculate_energy(false, true, \
                                                             input, length)
#define CUSTOM_FUNC_PEAK(input, length) calculate_energy(true, true, input, \
                                                         length)
#define NO_OUTPUT
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
