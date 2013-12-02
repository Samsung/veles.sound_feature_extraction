/*! @file energy.cc
 *  @brief Tests for src/primitives/energy.cc.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include <gtest/gtest.h>
#include "src/primitives/energy.h"
#include <simd/arithmetic-inl.h>

TEST(Energy, calculate_energy) {
  const int length = 510;
  alignas(std::alignment_of<__m256>::value) float array[length];
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
