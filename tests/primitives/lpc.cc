/*! @file lpc.cc
 *  @brief Tests for src/primitives/lpc.cc.
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

#include <cmath>
#include <gtest/gtest.h>
#include <simd/correlate.h>
#include <simd/memory.h>
#include <simd/arithmetic-inl.h>
#include "src/primitives/lpc.h"
#include "src/make_unique.h"

float CalculateLPCError(const float* ac, int length, const float* lpc) {
  float serr = 0;
  for (int i = 0; i < length - 1; i++) {
    float err = 0;
    for (int j = 0; j < i; j++) {
      err += ac[i - j] * lpc[j];
    }
    for (int j = i; j < length - 1; j++) {
      err += ac[j - i] * lpc[j];
    }
    err += ac[i + 1];
    serr += err * err;
  }
  return serr;
}

TEST(LPC, ldr_lpc) {
  const int length = 17;
  float array[length], acopy[length], autocorr[length * 2 - 1],
      vlpc[length - 1], plpc[length - 1];
  for (int i = 0; i < length; i++) {
    array[i] = (i + 1.f) / length;
  }
  memcpy(acopy, array, sizeof(acopy));
  cross_correlate_simd(true, array, length, acopy, length, autocorr);
  memmove(array, autocorr + length - 1, length * sizeof(array[0]));
  real_multiply_scalar(array, length, 1 / array[0], array);
  ldr_lpc(false, array, length, vlpc);
  ldr_lpc(true, array, length, plpc);
  memsetf(acopy, 0, length);
  float errv = CalculateLPCError(array, length, vlpc);
  float errp = CalculateLPCError(array, length, plpc);
  float err_fool = CalculateLPCError(array, length, acopy);
  ASSERT_NEAR(0.f, errv, 1e-10);
  ASSERT_NEAR(0.f, errp, 1e-10);
  ASSERT_GT(err_fool, 0);
  EXPECT_NEAR(-1.f, vlpc[0], 0.1f);
  for (int i = 1; i < length - 1; i++) {
    EXPECT_NEAR(0.f, vlpc[i], 0.02f);
  }
  EXPECT_NEAR(-1.f, plpc[0], 0.1f);
  for (int i = 1; i < length - 1; i++) {
    EXPECT_NEAR(0.f, plpc[i], 0.02f);
  }
}

#ifdef BENCHMARK
class LPCTest : public ::testing::TestWithParam<bool> {
 public:
  typedef std::unique_ptr<float[], decltype(&std::free)> FloatPtr;

 protected:
  LPCTest() : length_(256), iterations_(10000),
              input_(nullptr, std::free),
              output_(nullptr, std::free) {
  }

  virtual void SetUp() override {
    input_ = std::uniquify(mallocf(length_), std::free);
    for (int i = 0; i < length_; i++) {
      input_.get()[i] = i;
    }
    output_ = std::uniquify(mallocf(length_), std::free);
  }

  int length_;
  int iterations_;
  FloatPtr input_;
  FloatPtr output_;
};

TEST_P(LPCTest, ldr_lpc) {
  float error = 0;
  for (int i = 0; i < iterations_; i++) {
    input_.get()[i % length_] = i;
    error += ldr_lpc(GetParam(), input_.get(), length_, output_.get());
  }
  if (error != error) {
    error = 0;
  }
  if (error > 0) {
    error = 0;
  }
  EXPECT_GE(0, error);
}

INSTANTIATE_TEST_CASE_P(LPCBenchmark, LPCTest, ::testing::Bool());
#endif  // BENCHMARK

#include "tests/google/src/gtest_main.cc"
