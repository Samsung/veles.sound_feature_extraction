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


#include "src/transforms/frequency_bands.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::RawFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::FrequencyBands;

class FrequencyBandsTest : public TransformTest<FrequencyBands> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 512;
    SetParameter("number", "4");
    SetUpTransform(4, Size, 16000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i;
      (*Input)[1][i] = i;
      (*Input)[2][i] = i;
      (*Input)[3][i] = i;
    }
  }
};

#define EPSILON 0.000075f

TEST_F(FrequencyBandsTest, Do) {
  ASSERT_EQ(Size, inputFormat_->Size());
  ASSERT_EQ(Size, outputFormat_->Size());
  Do((*Input), &(*Output));
  for (int i = 0; i < Size / 4; i++) {
    ASSERT_EQ(i, (*Output)[0][i]);
    ASSERT_EQ(0, (*Output)[1][i]);
    ASSERT_EQ(0, (*Output)[2][i]);
    ASSERT_EQ(0, (*Output)[3][i]);
  }
  for (int i = Size / 4; i < 2 * Size / 4; i++) {
    ASSERT_EQ(0, (*Output)[0][i]);
    ASSERT_EQ(i, (*Output)[1][i]);
    ASSERT_EQ(0, (*Output)[2][i]);
    ASSERT_EQ(0, (*Output)[3][i]);
  }
  for (int i = 2 * Size / 4; i < 3 * Size / 4; i++) {
    ASSERT_EQ(0, (*Output)[0][i]);
    ASSERT_EQ(0, (*Output)[1][i]);
    ASSERT_EQ(i, (*Output)[2][i]);
    ASSERT_EQ(0, (*Output)[3][i]);
  }
  for (int i = 3 * Size / 4; i < Size; i++) {
    ASSERT_EQ(0, (*Output)[0][i]);
    ASSERT_EQ(0, (*Output)[1][i]);
    ASSERT_EQ(0, (*Output)[2][i]);
    ASSERT_EQ(i, (*Output)[3][i]);
  }
}

TEST_F(FrequencyBandsTest, DoWithBands) {
  SetParameter("bands", "2000 3000 5000 8000");
  Initialize();
  Do((*Input), &(*Output));
  for (int i = 0; i < Size / 4; i++) {
    ASSERT_EQ(i, (*Output)[0][i]);
    ASSERT_EQ(0, (*Output)[1][i]);
    ASSERT_EQ(0, (*Output)[2][i]);
    ASSERT_EQ(0, (*Output)[3][i]);
  }
  for (int i = Size / 4; i < 3 * Size / 8; i++) {
    ASSERT_EQ(0, (*Output)[0][i]);
    ASSERT_EQ(i, (*Output)[1][i]);
    ASSERT_EQ(0, (*Output)[2][i]);
    ASSERT_EQ(0, (*Output)[3][i]);
  }
  for (int i = 3 * Size / 8; i < 5 * Size / 8; i++) {
    ASSERT_EQ(0, (*Output)[0][i]);
    ASSERT_EQ(0, (*Output)[1][i]);
    ASSERT_EQ(i, (*Output)[2][i]);
    ASSERT_EQ(0, (*Output)[3][i]);
  }
  for (int i = 5 * Size / 8; i < Size; i++) {
    ASSERT_EQ(0, (*Output)[0][i]);
    ASSERT_EQ(0, (*Output)[1][i]);
    ASSERT_EQ(0, (*Output)[2][i]);
    ASSERT_EQ(i, (*Output)[3][i]);
  }
}

TEST_F(FrequencyBandsTest, TooBigBands) {
  SetParameter("bands", "2000 3000 5000 18000");
  Initialize();
  Do((*Input), &(*Output));
  for (int i = 0; i < Size / 4; i++) {
    ASSERT_EQ(i, (*Output)[0][i]);
    ASSERT_EQ(0, (*Output)[1][i]);
    ASSERT_EQ(0, (*Output)[2][i]);
    ASSERT_EQ(0, (*Output)[3][i]);
  }
  for (int i = Size / 4; i < 3 * Size / 8; i++) {
    ASSERT_EQ(0, (*Output)[0][i]);
    ASSERT_EQ(i, (*Output)[1][i]);
    ASSERT_EQ(0, (*Output)[2][i]);
    ASSERT_EQ(0, (*Output)[3][i]);
  }
  for (int i = 3 * Size / 8; i < 5 * Size / 8; i++) {
    ASSERT_EQ(0, (*Output)[0][i]);
    ASSERT_EQ(0, (*Output)[1][i]);
    ASSERT_EQ(i, (*Output)[2][i]);
    ASSERT_EQ(0, (*Output)[3][i]);
  }
  for (int i = 5 * Size / 8; i < Size; i++) {
    ASSERT_EQ(0, (*Output)[0][i]);
    ASSERT_EQ(0, (*Output)[1][i]);
    ASSERT_EQ(0, (*Output)[2][i]);
    ASSERT_EQ(i, (*Output)[3][i]);
  }
}

TEST_F(FrequencyBandsTest, InvalidBands) {
  ASSERT_THROW({
    SetParameter("bands", "2000 3000 15000 8000");
  }, SoundFeatureExtraction::InvalidParameterValueException);
}
