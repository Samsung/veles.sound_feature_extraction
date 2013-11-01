/*! @file short_time_msn.cc
 *  @brief Tests for sound_feature_extraction::transforms::ShortTimeMeanScaleNormalization.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include "src/transforms/short_time_msn.h"
#include "tests/transforms/transform_test.h"
#include <fftf/api.h>

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::ShortTimeMeanScaleNormalization;

class ShortTimeMeanScaleNormalizationTest : public TransformTest<ShortTimeMeanScaleNormalization> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 486;
    SetParameter("length", "5");
    SetUpTransform(10, Size, 18000);
    for (int k = 0; k < 10; k++) {
      for (int i = 0; i < Size; i++) {
        (*Input)[k][i] = i + k;
      }
    }
  }
};

TEST_F(ShortTimeMeanScaleNormalizationTest, Do) {
  Do((*Input), &(*Output));
  ASSERT_NEAR((*Output)[0][0], -0.5, 0.00001f);
  ASSERT_NEAR((*Output)[0][1], -0.5, 0.00001f);
  ASSERT_NEAR((*Output)[0][2], -0.5, 0.00001f);
  ASSERT_NEAR((*Output)[1][0], -0.16666f, 0.00001f);
  ASSERT_NEAR((*Output)[1][1], -0.16666f, 0.00001f);
  ASSERT_NEAR((*Output)[1][2], -0.16666f, 0.00001f);
  ASSERT_NEAR((*Output)[2][0], 0, 0.00001f);
  ASSERT_NEAR((*Output)[2][1], 0, 0.00001f);
  ASSERT_NEAR((*Output)[2][2], 0, 0.00001f);
  ASSERT_NEAR((*Output)[3][0], 0, 0.00001f);
  ASSERT_NEAR((*Output)[3][1], 0, 0.00001f);
  ASSERT_NEAR((*Output)[3][2], 0, 0.00001f);
  ASSERT_NEAR((*Output)[9][0], 0.5, 0.00001f);
  ASSERT_NEAR((*Output)[9][1], 0.5, 0.00001f);
  ASSERT_NEAR((*Output)[9][2], 0.5, 0.00001f);
}
