/*! @file real_to_complex.cc
 *  @brief Tests for SpeechFeatureExtraction::Transforms::RealToComplex.
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
#include "src/transforms/real_to_complex.h"

using SpeechFeatureExtraction::Formats::WindowF;
using SpeechFeatureExtraction::Formats::WindowFormatF;
using SpeechFeatureExtraction::BuffersBase;
using SpeechFeatureExtraction::Transforms::RealToComplex;

class RealToComplexTest : public RealToComplex, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  virtual void SetUp() {
    Size = 486;
    Input.Initialize(1, Size * 2);
    for (int i = 0; i < Size; i++) {
      Input[0]->Data.get()[i] = i;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    format->SetAllocatedSize(Size * 2);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(RealToComplexTest, Do) {
  Do(Input, &Output);
  for (int i = 0; i < Size; i++) {
    ASSERT_EQ(i, Output[0]->Data.get()[i * 2]);
  }
}

#define CLASS_NAME RealToComplexTest
#define ITER_COUNT 500000
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
