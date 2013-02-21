/*! @file square.cc
 *  @brief Tests for src/transforms/square.h.
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

using namespace SpeechFeatureExtraction::Formats;
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
   RawFormat16 format(32000, 16000);
   SetInputFormat(format);
   TypeSafeInitializeBuffers(Input, &Output);
  }
};

TEST_F(SquareRawTest, Do) {
  TypeSafeDo(Input, &Output);
  for (int i = 0; i < 32000; i++) {
    ASSERT_EQ(i * i, Output[0]->Data.get()[i]);
  }
}

#include "tests/google/src/gtest_main.cc"
