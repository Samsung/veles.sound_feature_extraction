/*! @file square.cc
 *  @brief Tests for SpeechFeatureExtraction::Transforms::SquareRaw.
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

using SpeechFeatureExtraction::Formats::Raw16;
using SpeechFeatureExtraction::Formats::Raw32;
using SpeechFeatureExtraction::Formats::RawFormat16;
using SpeechFeatureExtraction::BuffersBase;
using SpeechFeatureExtraction::Transforms::SquareRaw;

class SquareRawTest : public SquareRaw, public testing::Test {
 public:
  BuffersBase<Raw16> Input;
  BuffersBase<Raw32> Output;

  virtual void SetUp() {
    Input.Initialize(1, 32000, 0);
    for (int i = 0; i < 32000; i++) {
      Input[0]->Data.get()[i] = i;
    }
    auto format = std::make_shared<RawFormat16>(32000, 16000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(SquareRawTest, Do) {
  Do(Input, &Output);
  for (int i = 0; i < 32000; i++) {
    ASSERT_EQ(i * i, Output[0]->Data.get()[i]);
  }
}

#include "tests/google/src/gtest_main.cc"
