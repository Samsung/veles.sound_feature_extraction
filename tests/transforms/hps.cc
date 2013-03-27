/*! @file hps.cc
 *  @brief Tests for SpeechFeatureExtraction::Transforms::HPS.
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
#include "src/transforms/hps.h"
#include "tests/transforms/hps_test.inc"

using SpeechFeatureExtraction::Formats::WindowF;
using SpeechFeatureExtraction::Formats::WindowFormatF;
using SpeechFeatureExtraction::BuffersBase;
using SpeechFeatureExtraction::Transforms::Hps;

class HpsTest : public Hps, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<float> Output;
  int Size;

  virtual void SetUp() {
    Size = data[1];
    Input.Initialize(1, Size);
    for (int i = 0; i < data[1]; ++i) {
      Input[0]->Data.get()[i] = data[i + 2];
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    format->SetSamplingRate(data[0]);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(HpsTest, Do) {
  Do(Input, &Output);
  float delta = *Output[0] - 110.671507f;
  ASSERT_LT(delta * delta, 1.0f);
}

#include "tests/google/src/gtest_main.cc"
