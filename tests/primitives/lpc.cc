/*! @file lpc.cc
 *  @brief Tests for src/primitives/lpc.cc.
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
#include <simd/memory.h>
#include "src/primitives/lpc.h"
#include "src/make_unique.h"

TEST(LPC, ldr_lpc) {
  const int length = 64;
  float array[length], vlpc[length - 1], plpc[length - 1];
  array[0] = 1;
  for (int i = 1; i < length; i++) {
    array[i] = sinf(i * M_PI / 100);
  }
  ldr_lpc(false, array, length, vlpc);
  ldr_lpc(true, array, length, plpc);
  for (int i = 0; i < length - 1; i++) {
    ASSERT_NEAR(vlpc[i], plpc[i], fabs(vlpc[i])) << i;
  }

}

#ifdef BENCHMARK
class LPCTest : public ::testing::TestWithParam<bool> {
 public:
  typedef std::unique_ptr<float, decltype(&std::free)> FloatPtr;

 protected:
  LPCTest() : length_(128), iterations_(20000),
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
