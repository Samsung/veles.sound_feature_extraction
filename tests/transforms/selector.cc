/*! @file selector.cc
 *  @brief Tests for SpeechFeatureExtraction::Transforms::Selector.
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
#include "src/transforms/selector.h"

using SpeechFeatureExtraction::Formats::WindowF;
using SpeechFeatureExtraction::Formats::WindowFormatF;
using SpeechFeatureExtraction::BuffersBase;
using SpeechFeatureExtraction::Transforms::Selector;

class SelectorTest : public Selector, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  virtual void SetUp() {
    SetParameter("from", "left");
    SetParameter("length", "6");
    Size = 512;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      Input[0]->Data.get()[i] = i;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 16000, 16000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(SelectorTest, Do) {
  ASSERT_EQ(6, outputFormat_->Size());
  Do(Input, &Output);
  ASSERT_EQ(0, memcmp(Input[0]->Data.get(),
                      Output[0]->Data.get(),
                      6 * sizeof(float)));  // NOLINT(*)
  SetParameter("from", "right");
  Do(Input, &Output);
  ASSERT_EQ(0, memcmp(Input[0]->Data.get() + 512 - 6,
                      Output[0]->Data.get(),
                      6 * sizeof(float)));  // NOLINT(*)
}

#include "tests/google/src/gtest_main.cc"
