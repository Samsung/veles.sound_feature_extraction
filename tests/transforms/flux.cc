/*! @file flux.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Flux.
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
#include "src/transforms/flux.h"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Flux;

class FluxTest : public Flux, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<float> Output;
  static const int Size;

  FluxTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Input.Initialize(2, Size);
    for (int i = 0; i < Size; i++) {
      Input[0]->Data.get()[i] = fabs(sinf(i * i) + i * cosf(i));
      Input[1]->Data.get()[i] = 0.f;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

const int FluxTest::Size = 450;

#define EPSILON 0.0001f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(FluxTest, Do) {
  Do(Input, &Output);
  float res = Do(false, Input[1]->Data.get(), Size, Input[0]->Data.get());
  ASSERT_EQ(0.f, *Output[0]);
  ASSERT_EQF(res, *Output[1]);
}

const float extra_param[FluxTest::Size] = { 0.f };

#define CLASS_NAME FluxTest
#define ITER_COUNT 400000
#define NO_OUTPUT
#define EXTRA_PARAM extra_param
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
