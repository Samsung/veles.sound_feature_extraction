/*! @file short_time_average.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::ShortTimeAverage.
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
#include "src/transforms/short_time_average.h"
#include <fftf/api.h>

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::ShortTimeAverage;

class ShortTimeAverageTest
    : public ShortTimeAverage, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  ShortTimeAverageTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 486;
    Input.Initialize(10, Size);
    for (int k = 0; k < 10; k++) {
      for (int i = 0; i < Size; i++) {
        Input[k].Data.get()[i] = i + k;
      }
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
    SetParameter("length", "5");
  }
};

TEST_F(ShortTimeAverageTest, Do) {
  Do(Input, &Output);
  ASSERT_NEAR(Output[0].Data.get()[0], 1, 0.00001f);
  ASSERT_NEAR(Output[0].Data.get()[1], 2, 0.00001f);
  ASSERT_NEAR(Output[0].Data.get()[2], 3, 0.00001f);
  ASSERT_NEAR(Output[1].Data.get()[0], 1.5f, 0.00001f);
  ASSERT_NEAR(Output[1].Data.get()[1], 2.5f, 0.00001f);
  ASSERT_NEAR(Output[1].Data.get()[2], 3.5f, 0.00001f);
  ASSERT_NEAR(Output[2].Data.get()[0], 2, 0.00001f);
  ASSERT_NEAR(Output[2].Data.get()[1], 3, 0.00001f);
  ASSERT_NEAR(Output[2].Data.get()[2], 4, 0.00001f);
  ASSERT_NEAR(Output[3].Data.get()[0], 3, 0.00001f);
  ASSERT_NEAR(Output[3].Data.get()[1], 4, 0.00001f);
  ASSERT_NEAR(Output[3].Data.get()[2], 5, 0.00001f);
  ASSERT_NEAR(Output[9].Data.get()[0], 8, 0.00001f);
  ASSERT_NEAR(Output[9].Data.get()[1], 9, 0.00001f);
  ASSERT_NEAR(Output[9].Data.get()[2], 10, 0.00001f);
}

#include "tests/google/src/gtest_main.cc"
