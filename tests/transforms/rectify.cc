/*! @file rectify.cc
 *  @brief Tests for SpeechFeatureExtraction::Transforms::RECTIFY.
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
#include "src/transforms/rectify.h"

using SpeechFeatureExtraction::Formats::WindowF;
using SpeechFeatureExtraction::Formats::WindowFormatF;
using SpeechFeatureExtraction::BuffersBase;
using SpeechFeatureExtraction::Transforms::Rectify;

class RectifyTest : public Rectify, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  virtual void SetUp() {
    Size = 486;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; ++i) {
      Input[0]->Data.get()[i] = i;
      if (i % 2 == 0) {
        Input[0]->Data.get()[i] *= -1;
      }
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    format->SetAllocatedSize(2 * Size);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(RectifyTest, Do) {
  Do(Input, &Output);
  for (int i = 0; i < Size; i++) {
    ASSERT_EQ(i, Output[0]->Data.get()[i]);
  }
}

#define CLASS_NAME RectifyTest
#define ITER_COUNT 500000
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
