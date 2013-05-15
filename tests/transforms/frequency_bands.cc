/*! @file frequency_bands.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::FrequencyBands.
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
#include "src/transforms/frequency_bands.h"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::FrequencyBands;

class FrequencyBandsTest : public FrequencyBands, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  FrequencyBandsTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 512;
    SetParameter("number", "4");
    Input.Initialize(4, Size);
    for (int i = 0; i < Size; i++) {
      Input[0]->Data.get()[i] = i;
      Input[1]->Data.get()[i] = i;
      Input[2]->Data.get()[i] = i;
      Input[3]->Data.get()[i] = i;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 16000, 16000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

#define EPSILON 0.000075f

TEST_F(FrequencyBandsTest, Do) {
  ASSERT_EQ(Size, inputFormat_->Size());
  ASSERT_EQ(Size, outputFormat_->Size());
  Do(Input, &Output);
  for (int i = 0; i < Size / 4; i++) {
    ASSERT_EQ(i, Output[0]->Data.get()[i]);
    ASSERT_EQ(0, Output[1]->Data.get()[i]);
    ASSERT_EQ(0, Output[2]->Data.get()[i]);
    ASSERT_EQ(0, Output[3]->Data.get()[i]);
  }
  for (int i = Size / 4; i < 2 * Size / 4; i++) {
    ASSERT_EQ(0, Output[0]->Data.get()[i]);
    ASSERT_EQ(i, Output[1]->Data.get()[i]);
    ASSERT_EQ(0, Output[2]->Data.get()[i]);
    ASSERT_EQ(0, Output[3]->Data.get()[i]);
  }
  for (int i = 2 * Size / 4; i < 3 * Size / 4; i++) {
    ASSERT_EQ(0, Output[0]->Data.get()[i]);
    ASSERT_EQ(0, Output[1]->Data.get()[i]);
    ASSERT_EQ(i, Output[2]->Data.get()[i]);
    ASSERT_EQ(0, Output[3]->Data.get()[i]);
  }
  for (int i = 3 * Size / 4; i < Size; i++) {
    ASSERT_EQ(0, Output[0]->Data.get()[i]);
    ASSERT_EQ(0, Output[1]->Data.get()[i]);
    ASSERT_EQ(0, Output[2]->Data.get()[i]);
    ASSERT_EQ(i, Output[3]->Data.get()[i]);
  }
}

TEST_F(FrequencyBandsTest, DoWithBands) {
  SetParameter("bands", "2000 3000 5000 8000");
  Do(Input, &Output);
  for (int i = 0; i < Size / 4; i++) {
    ASSERT_EQ(i, Output[0]->Data.get()[i]);
    ASSERT_EQ(0, Output[1]->Data.get()[i]);
    ASSERT_EQ(0, Output[2]->Data.get()[i]);
    ASSERT_EQ(0, Output[3]->Data.get()[i]);
  }
  for (int i = Size / 4; i < 3 * Size / 8; i++) {
    ASSERT_EQ(0, Output[0]->Data.get()[i]);
    ASSERT_EQ(i, Output[1]->Data.get()[i]);
    ASSERT_EQ(0, Output[2]->Data.get()[i]);
    ASSERT_EQ(0, Output[3]->Data.get()[i]);
  }
  for (int i = 3 * Size / 8; i < 5 * Size / 8; i++) {
    ASSERT_EQ(0, Output[0]->Data.get()[i]);
    ASSERT_EQ(0, Output[1]->Data.get()[i]);
    ASSERT_EQ(i, Output[2]->Data.get()[i]);
    ASSERT_EQ(0, Output[3]->Data.get()[i]);
  }
  for (int i = 5 * Size / 8; i < Size; i++) {
    ASSERT_EQ(0, Output[0]->Data.get()[i]);
    ASSERT_EQ(0, Output[1]->Data.get()[i]);
    ASSERT_EQ(0, Output[2]->Data.get()[i]);
    ASSERT_EQ(i, Output[3]->Data.get()[i]);
  }
}

TEST_F(FrequencyBandsTest, TooBigBands) {
  SetParameter("bands", "2000 3000 5000 18000");
  Do(Input, &Output);
  for (int i = 0; i < Size / 4; i++) {
    ASSERT_EQ(i, Output[0]->Data.get()[i]);
    ASSERT_EQ(0, Output[1]->Data.get()[i]);
    ASSERT_EQ(0, Output[2]->Data.get()[i]);
    ASSERT_EQ(0, Output[3]->Data.get()[i]);
  }
  for (int i = Size / 4; i < 3 * Size / 8; i++) {
    ASSERT_EQ(0, Output[0]->Data.get()[i]);
    ASSERT_EQ(i, Output[1]->Data.get()[i]);
    ASSERT_EQ(0, Output[2]->Data.get()[i]);
    ASSERT_EQ(0, Output[3]->Data.get()[i]);
  }
  for (int i = 3 * Size / 8; i < 5 * Size / 8; i++) {
    ASSERT_EQ(0, Output[0]->Data.get()[i]);
    ASSERT_EQ(0, Output[1]->Data.get()[i]);
    ASSERT_EQ(i, Output[2]->Data.get()[i]);
    ASSERT_EQ(0, Output[3]->Data.get()[i]);
  }
  for (int i = 5 * Size / 8; i < Size; i++) {
    ASSERT_EQ(0, Output[0]->Data.get()[i]);
    ASSERT_EQ(0, Output[1]->Data.get()[i]);
    ASSERT_EQ(0, Output[2]->Data.get()[i]);
    ASSERT_EQ(i, Output[3]->Data.get()[i]);
  }
}

TEST_F(FrequencyBandsTest, InvalidBands) {
  ASSERT_THROW({
    SetParameter("bands", "2000 3000 15000 8000");
  }, SoundFeatureExtraction::InvalidParameterValueException);
}

#include "tests/google/src/gtest_main.cc"
