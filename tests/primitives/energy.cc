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
  float array[length] __attribute__((aligned(64)));
  for (int i = 0; i < length; i++) {
    array[i] = i + 1;
  }
  float result = calculate_energy(true, true, array, length);
  ASSERT_NEAR((length + 1) * (2 * length + 1.0f) / 6, result, 0.01f);
}

#define TEST_NAME Energy
#define ITER_COUNT 500000
#define BENCH_FUNC calculate_energy
#define NO_OUTPUT
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
