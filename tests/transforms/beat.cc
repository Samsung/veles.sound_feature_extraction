/*! @file beat.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::BEAT.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
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
#include "src/transforms/beat.h"
#include "tests/transforms/beat_test.inc"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Beat;

class BeatTest : public Beat, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<float> Output;
  int Size;

  BeatTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = data[0];
    Input.Initialize(1, Size);
    for (int i = 0; i < data[0]; ++i) {
      Input[0].Data.get()[i] = data[i + 1];
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(BeatTest, Do) {
  Do(Input, &Output);
  float delta = 122.85f - Output[0];
  if (delta * delta > 1E-1) {
    ASSERT_EQ(-1, Output[0]);
  }
}

#include "tests/google/src/gtest_main.cc"
