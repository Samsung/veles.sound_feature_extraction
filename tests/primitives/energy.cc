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

#define EPSILON 0.005f

#define ASSERT_EQF(a, b) do { \
  ASSERT_GT(a + EPSILON, b); \
  ASSERT_LT(a - EPSILON, b); \
} while (0)

TEST(Energy, calculate_energy) {
  float array[510] __attribute__((aligned(32)));
  int length = sizeof(array) / sizeof(float);
  for (int i = 0; i < length; i++) {
    array[i] = i + 1;
  }
  float result = calculate_energy(array, length);
  ASSERT_EQF((length + 1) * (2 * length + 1.0f) / 6, result);
}

#include "tests/google/src/gtest_main.cc"
