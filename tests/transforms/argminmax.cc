/*! @file complex_to_real.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::ArgMinMax.
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
#include "src/transforms/argminmax.h"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::ArgMinMax;
using SoundFeatureExtraction::Transforms::ArgMinMaxResult;

class ArgMinMaxTest
    : public ArgMinMax, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<ArgMinMaxResult> Output;
  int Size;

  ArgMinMaxTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 486;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size / 2; i++) {
      Input[0].Data.get()[i] = -i + 1;
    }
    for (int i = Size / 2; i < Size; i++) {
      Input[0].Data.get()[i] = Size - i + 1;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

#define EPSILON 0.15f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(ArgMinMaxTest, Do) {
  SetParameter("extremum", "max");
  Do(Input[0], &Output[0]);
  ASSERT_EQ(Size / 2, std::get<0>(Output[0]));
  ASSERT_EQ(Size / 2 + 1, std::get<1>(Output[0]));
  SetParameter("extremum", "min");
  Do(Input[0], &Output[0]);
  ASSERT_EQ(Size / 2 - 1, std::get<0>(Output[0]));
  ASSERT_EQ(- Size / 2 + 2, std::get<1>(Output[0]));
}

#define EXTRA_PARAM true
#define CLASS_NAME ArgMinMaxTest
#define ITER_COUNT 500000
#define NO_OUTPUT
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
