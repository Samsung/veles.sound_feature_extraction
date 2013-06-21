/*! @file diffrect.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Diffrect.
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
#include "src/transforms/diffrect.h"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::DiffrectWindow;

class DiffrectTest
    : public DiffrectWindow, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  DiffrectTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 486;
    Input.Initialize(1, 2 * Size);
    Input[0][0] = 0;
    for (int i = 1; i < Size; i++) {
      int step = (i - 1) % 3;
      if ((i - 1) % 2 == 0)
        step *= -1;
      Input[0][i] = Input[0][i - 1] + step;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    format->SetAllocatedSize(2 * Size);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(DiffrectTest, Do) {
  Do(Input[0], &Output[0]);
  for (int i = 0; i < Size - 1; i++) {
    int result = (i % 3);
    if (i % 2 == 0) {
      result = 0;
    }
    ASSERT_EQ(result, Output[0][i]);
  }
}

#define CLASS_NAME DiffrectTest
#define ITER_COUNT 500000
#define BENCH_FUNC DiffrectBase::Do
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
