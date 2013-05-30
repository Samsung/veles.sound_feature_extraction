/*! @file complex_to_real.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::ComplexToReal.
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
#include "src/transforms/complex_to_real.h"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::ComplexToReal;

class ComplexToRealTest
    : public ComplexToReal, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  ComplexToRealTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 486;
    Input.Initialize(1, 2 * Size);
    for (int i = 0; i < Size; i++) {
      Input[0][i] = i;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    format->SetAllocatedSize(2 * Size);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(ComplexToRealTest, Do) {
  Do(Input[0], &Output[0]);
  for (int i = 0; i < Size / 2; i++) {
    int result = 2 * i;
    ASSERT_EQ(result, Output[0][i]);
  }
}

#define CLASS_NAME ComplexToRealTest
#define ITER_COUNT 800000
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
