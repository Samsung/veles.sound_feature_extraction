/*! @file hamming_window_filter.cc
 *  @brief Tests for src/transforms/hamming_window_filter.cc.
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

#define EPSILON 0.0001f

#define ASSERT_EQF(a, b) do { \
  ASSERT_GT(a + EPSILON, b); \
  ASSERT_LT(a - EPSILON, b); \
} while(0)

using SpeechFeatureExtraction::Transforms::LowpassFilter;

class HammingWindowFilterTest : public LowpassFilter, public testing::Test {
};

TEST_F(HammingWindowFilterTest, WindowFunction) {
  std::vector<float> filter(4);
  std::vector<float> valid { 0.0800f, 0.7700f, 0.7700f, 0.0800f };
  length_ = 4;
  for (size_t i = 0; i < filter.size(); i++) {
    filter[i] = WindowFunction(i);
  }
  for (size_t i = 0; i < filter.size(); i++) {
    ASSERT_EQF(valid[i], filter[i]);
  }

  filter.resize(5);
  valid = { 0.0800f, 0.5400f, 1.0000f, 0.5400f, 0.0800f };
  length_ = 5;
  for (size_t i = 0; i < filter.size(); i++) {
    filter[i] = WindowFunction(i);
  }
  for (size_t i = 0; i < filter.size(); i++) {
    ASSERT_EQF(valid[i], filter[i]);
  }

  filter.resize(19);
  valid = { 0.0800f, 0.1077f, 0.1876f, 0.3100f, 0.4601f,
            0.6199f, 0.7700f, 0.8924f, 0.9723f, 1.0000f,
            0.9723f, 0.8924f, 0.7700f, 0.6199f, 0.4601f,
            0.3100f, 0.1876f, 0.1077f, 0.0800f };
  length_ = 19;
  for (size_t i = 0; i < filter.size(); i++) {
    filter[i] = WindowFunction(i);
  }
  for (size_t i = 0; i < filter.size(); i++) {
    ASSERT_EQF(valid[i], filter[i]);
  }

  filter.resize(20);
  valid = { 0.0800f, 0.1049f, 0.1770f, 0.2884f, 0.4271f,
            0.5780f, 0.7248f, 0.8515f, 0.9446f, 0.9937f,
            0.9937f, 0.9446f, 0.8515f, 0.7248f, 0.5780f,
            0.4271f, 0.2884f, 0.1770f, 0.1049f, 0.0800f };
  length_ = 20;
  for (size_t i = 0; i < filter.size(); i++) {
    filter[i] = WindowFunction(i);
  }
  for (size_t i = 0; i < filter.size(); i++) {
    ASSERT_EQF(valid[i], filter[i]);
  }
}

#include "tests/google/src/gtest_main.cc"
