/*! @file zerocrossings.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::ZeroCrossings.
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
#include "src/transforms/zerocrossings.h"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::Formats::Raw16;
using SoundFeatureExtraction::Formats::RawFormat16;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::ZeroCrossingsWindow;
using SoundFeatureExtraction::Transforms::ZeroCrossingsRaw;

class ZeroCrossingsWindowTest : public ZeroCrossingsWindow, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<int32_t> Output;
  int Size;

  ZeroCrossingsWindowTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 450;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      // Always liked exotic functions
      Input[0].Data.get()[i] = sinf(i * M_PI / 2);
    }
    Input[0].Data.get()[Size - 1] = 0;
    Input[0].Data.get()[Size - 2] = 0;
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(ZeroCrossingsWindowTest, Do) {
  Do(Input[0], &Output[0]);
  ASSERT_EQ(Size / 2 + 1, Output[0]);
  int slowres = Do(false, Input[0].Data.get(), Size);
  ASSERT_EQ(Size / 2 + 1, slowres);
}

#define CLASS_NAME ZeroCrossingsWindowTest
#define ITER_COUNT 400000
#define NO_OUTPUT
#include "tests/transforms/benchmark.inc"

class ZeroCrossingsRawTest : public ZeroCrossingsRaw, public testing::Test {
 public:
  BuffersBase<Raw16> Input;
  BuffersBase<int32_t> Output;
  int Size;

  ZeroCrossingsRawTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 1024;
    Input.Initialize(1, Size, 2);
    for (int i = 0; i < Size; i++) {
      // Always liked exotic functions
      Input[0].Data.get()[i] = sinf(i * M_PI / 2) * 1024;
    }
    Input[0].Data.get()[3] = 1024;
    auto format = std::make_shared<RawFormat16>(Size, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(ZeroCrossingsRawTest, Do) {
  Do(Input, &Output);
  ASSERT_EQ(Size / 2, Output[0]);
  int slowres = Do(false, Input[0].Data.get(), Size);
  ASSERT_EQ(Size / 2, slowres);
}

#undef CLASS_NAME
#define CLASS_NAME ZeroCrossingsRawTest
#undef INPUT_TYPE
#define INPUT_TYPE int16_t
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
