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

using SoundFeatureExtraction::Formats::ArrayFormatF;
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
      (*Input)[0][i] = sinf(i / 10.f);
      (*Input)[1][i] = sinf(i / 10.f);
      (*Input)[2][i] = sinf(i / 10.f);
      (*Input)[3][i] = sinf(i / 10.f);
    }
  }
};

TEST_F(FrequencyBandsTest, Do) {
  ASSERT_EQ(static_cast<size_t>(Size), inputFormat_->Size());
  ASSERT_EQ(static_cast<size_t>(Size), outputFormat_->Size());
  Do((*Input), &(*Output));
}

TEST_F(FrequencyBandsTest, DoWithBands) {
  SetParameter("bands", "2000 3000 5000");
  Initialize();
  Do((*Input), &(*Output));
}

TEST_F(FrequencyBandsTest, TooBigBands) {
  SetParameter("bands", "2000 3000 18000");
  Initialize();
  ASSERT_EQ(3, bands_number());
}

TEST_F(FrequencyBandsTest, InvalidBands) {
  ASSERT_THROW({
    SetParameter("bands", "2000 15000 8000");
  }, SoundFeatureExtraction::InvalidParameterValueException);
}
