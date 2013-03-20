/*! @file intensity.cc
 *  @brief Tests for SpeechFeatureExtraction::Transforms::Intensity.
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
#include <math.h>
#include "src/transforms/intensity.h"

using SpeechFeatureExtraction::Formats::WindowF;
using SpeechFeatureExtraction::Formats::WindowFormatF;
using SpeechFeatureExtraction::BuffersBase;
using SpeechFeatureExtraction::Transforms::Intensity;

class IntensityTest : public Intensity, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<float> Output;
  int Size;

  virtual void SetUp() {
    Size = 378;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      Input[0]->Data.get()[i] = i / 40.0f;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

#define EPSILON 0.000075f

#define ASSERT_EQF(a, b) do { \
  ASSERT_GT(a + EPSILON, b); \
  ASSERT_LT(a - EPSILON, b); \
} while (0)

TEST_F(IntensityTest, Do) {
  Do(Input, &Output);
  double res = .0;
  for (int i = 0; i < Size; i++) {
    float tmp = Input[0]->Data.get()[i];
    res += tmp * tmp;
  }
  ASSERT_EQF(logf(res / Size), *Output[0]);
}

#include "tests/google/src/gtest_main.cc"
