/*! @file square.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::SquareRaw.
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
#include "src/transforms/square.h"

using SoundFeatureExtraction::Formats::Raw16;
using SoundFeatureExtraction::Formats::Raw32;
using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::RawFormat16;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::SquareRaw;
using SoundFeatureExtraction::Transforms::SquareWindow;

class SquareRawTest : public SquareRaw, public testing::Test {
 public:
  BuffersBase<Raw16> Input;
  BuffersBase<Raw32> Output;

  SquareRawTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Input.Initialize(1, 32000, 0);
    for (int i = 0; i < 32000; i++) {
      Input[0][i] = i;
    }
    auto format = std::make_shared<RawFormat16>(32000, 16000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

class SquareWindowTest : public SquareWindow, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  SquareWindowTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 378;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      Input[0][i] = i;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(SquareRawTest, Do) {
  Do(Input, &Output);
  for (int i = 0; i < 32000; i++) {
    ASSERT_EQ(i * i, Output[0][i]);
  }
}

#define EPSILON 0.01f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(SquareWindowTest, Do) {
  Do(Input[0], &Output[0]);
  for (int i = 0; i < Size; i++) {
    ASSERT_EQF(i * i, Output[0][i]);
  }
}

#define CLASS_NAME SquareWindowTest
#define ITER_COUNT 300000
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
