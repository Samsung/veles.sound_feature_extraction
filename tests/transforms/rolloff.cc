/*! @file rolloff.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Rolloff.
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
#include "src/transforms/rolloff.h"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Rolloff;

class RolloffTest : public Rolloff, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<float> Output;
  int Size;

  RolloffTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 450;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      // Always liked exotic functions
      Input[0].Data.get()[i] = fabs(sinf(i * i) + i * cosf(i));
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

#define EPSILON 0.0025f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(RolloffTest, Do) {
  SetParameter("ratio", "0.8");
  Do(Input[0], &Output[0]);
  float res = Do(false, Input[0].Data.get(), Size, 0.8f);
  res *= inputFormat_->SamplingRate() / (2.f * inputFormat_->SamplesCount());
  ASSERT_EQF(res, Output[0]);
}

#define CLASS_NAME RolloffTest
#define ITER_COUNT 400000
#define NO_OUTPUT
#define EXTRA_PARAM 0.85f
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
