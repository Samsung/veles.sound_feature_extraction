/*! @file lowpass_filter.cc
 *  @brief Tests for src/parameters.h.
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
#include "src/transforms/lowpass_filter.h"
#include "src/primitives/window.h"

#define EPSILON 0.0001f

#define ASSERT_EQF(a, b) do { \
  ASSERT_GT(a + EPSILON, b); \
  ASSERT_LT(a - EPSILON, b); \
} while(0)

using SpeechFeatureExtraction::Transforms::LowpassFilter;

class LowpassFilterTest : public LowpassFilter, public testing::Test {
 public:
  virtual void SetUp() {
    SetParameter("frequency", "600");
  }
};

TEST_F(LowpassFilterTest, CalculateLowpassFilter) {
  length_ = 4;
  inputFormat_.SetSamplingRate(16000);
  std::vector<float> filter(length_);
  for (size_t i = 0; i < filter.size(); i++) {
    filter[i] = WindowElement(WINDOW_TYPE_HAMMING, length_, i);
  }
  CalculateFilter(&filter[0]);
  ASSERT_EQF(0.0059f, filter[0]);
  ASSERT_EQF(0.0576f, filter[1]);
  ASSERT_EQF(0.0576f, filter[2]);
  ASSERT_EQF(0.0059f, filter[3]);

  length_ = 5;
  filter.resize(length_);
  for (size_t i = 0; i < filter.size(); i++) {
    filter[i] = WindowElement(WINDOW_TYPE_HAMMING, length_, i);
  }
  CalculateFilter(&filter[0]);
  ASSERT_EQF(0.0058f, filter[0]);
  ASSERT_EQF(0.0401f, filter[1]);
  ASSERT_EQF(0.0750f, filter[2]);
  ASSERT_EQF(0.0401f, filter[3]);
  ASSERT_EQF(0.0058f, filter[4]);
}

#include "tests/google/src/gtest_main.cc"
