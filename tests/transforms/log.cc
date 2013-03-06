/*! @file log.cc
 *  @brief Tests for SpeechFeatureExtraction::Transforms::Log.
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
#include "src/transforms/log.h"

using namespace SpeechFeatureExtraction::Formats;
using SpeechFeatureExtraction::BuffersBase;
using SpeechFeatureExtraction::Transforms::Log;

class LogTest : public Log, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  virtual void SetUp() {
    Size = 378;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      Input[0]->Data.get()[i] = (i + Size / 2.0f) / Size;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    TypeSafeInitializeBuffers(Input, &Output);
  }
};

#define EPSILON 0.000075f

#define ASSERT_EQF(a, b) do { \
  ASSERT_GT(a + EPSILON, b); \
  ASSERT_LT(a - EPSILON, b); \
} while (0)

TEST_F(LogTest, Do) {
  TypeSafeDo(Input, &Output);
  for (int i = 0; i < Size; i++) {
    float log = Output[0]->Data.get()[i];
    float vlog = logf((i + Size / 2.0f) / Size);
    ASSERT_EQF(vlog, log);
  }
}

#include "tests/google/src/gtest_main.cc"

