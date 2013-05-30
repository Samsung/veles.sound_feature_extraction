/*! @file autocorrelation.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Autocorrelation.
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
#include "src/transforms/autocorrelation.h"
#include <fftf/api.h>

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Autocorrelation;

class AutocorrelationTest
    : public Autocorrelation, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  AutocorrelationTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 486;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size / 2; i++) {
      Input[0][i] = -i + 1;
    }
    for (int i = Size / 2; i < Size; i++) {
      Input[0][i] = Size - i + 1;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
    Initialize();
  }
};

TEST_F(AutocorrelationTest, Do) {
  Do(Input[0], &Output[0]);
  ASSERT_NEAR(Output[0][0], 2, 1.f);
  ASSERT_NEAR(Output[0][1], 3, 1.f);
  ASSERT_NEAR(Output[0][3], -2, 1.f);
  ASSERT_NEAR(Output[0][200], -1.353e+06, 0.001e+06);
}

#include "tests/google/src/gtest_main.cc"
