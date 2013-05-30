/*! @file delta.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Delta.
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
#include "src/transforms/delta.h"
#include <fftf/api.h>

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Delta;

class DeltaTest
    : public Delta, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  DeltaTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 486;
    Input.Initialize(2, Size);
    for (int i = 0; i < Size; i++) {
      Input[0][i] = i;
      Input[1][i] = i + 1;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(DeltaTest, Do) {
  ASSERT_EQ(1, Output.Size());
  Do(Input, &Output);
  for (int i = 0; i < Size; i++) {
    ASSERT_NEAR(Output[0][i], 1, 0.00001f);
  }
}

#include "tests/google/src/gtest_main.cc"
