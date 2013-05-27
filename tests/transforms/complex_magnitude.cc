/*! @file complex_magnitude.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::ComplexMagnitude.
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
#include "src/transforms/complex_magnitude.h"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::ComplexMagnitude;

class ComplexMagnitudeTest : public ComplexMagnitude, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  ComplexMagnitudeTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 378;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      Input[0].Data.get()[i] = i;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

#define EPSILON 0.000075f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(ComplexMagnitudeTest, Do) {
  Do(Input[0], &Output[0]);
  for (int i = 0; i < Size / 2; i++) {
    float m = Output[0].Data.get()[i];
    float re = i * 2;
    float im = i * 2 + 1;
    ASSERT_EQF(sqrtf(re * re + im * im), m);
  }
}

#define CLASS_NAME ComplexMagnitudeTest
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
