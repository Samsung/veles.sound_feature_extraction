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


#include <cmath>
#include <gtest/gtest.h>
#include <simd/correlate.h>
#include <simd/memory.h>
#include <simd/arithmetic-inl.h>
#include "src/primitives/lsp.h"
#include "src/primitives/lpc.h"

TEST(LSP, lpc_to_lsp) {
  const int length = 64;
  float array[length * 2], acopy[length], lpc[length - 1], freq[length] = {0};
  for (int i = 0; i < length; i++) {
    array[i] = sinf(i * M_PI / 10);
  }
  memcpy(acopy, array, sizeof(acopy));
  cross_correlate_simd(true, array, length, acopy, length, array);
  memmove(array, array + length - 1, length * sizeof(float));
  real_multiply_scalar(array, length, 1 / array[0], array);
  ldr_lpc(true, array, length, lpc);
  int roots = lpc_to_lsp(false, lpc, 16, 16, 0.01f, freq);
  ASSERT_EQ(12, roots);
  float valid_roots[] { 0.185900524, 0.323033303, 0.551158667,
                        0.790174425, 0.895356715, 1.21053171,
                        1.25504351, 1.60880125, 1.62396896,
                        2.38771534, 2.75880361, 2.78210378 };
  for (int i = 0; i < 12; i++) {
    ASSERT_NEAR(valid_roots[i], freq[i], 0.01f);
  }
  roots = lpc_to_lsp(true, lpc, 16, 16, 0.01f, freq);
  ASSERT_EQ(12, roots);
  for (int i = 0; i < 12; i++) {
    ASSERT_NEAR(valid_roots[i], freq[i], 0.01f);
  }
}


#include "tests/google/src/gtest_main.cc"
