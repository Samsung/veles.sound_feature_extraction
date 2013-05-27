/*! @file fork.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Fork.
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
#include "src/transforms/fork.h"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Fork;

class ForkTest : public Fork, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  ForkTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 512;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      Input[0].Data.get()[i] = (i - Size / 2.0f) / Size;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 16000, 16000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
    Initialize();
  }
};

TEST_F(ForkTest, Forward) {
  ASSERT_EQ(inputFormat_->Size(), outputFormat_->Size());
  Do(Input, &Output);
  ASSERT_EQ(Input.Size() * kDefaultFactor, Output.Size());
}

#include "tests/google/src/gtest_main.cc"
