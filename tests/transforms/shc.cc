/*! @file shc.cc
 *  @brief Tests for sound_feature_extraction::transforms::SHC.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <math.h>
#include <simd/detect_peaks.h>
#include "src/transforms/shc.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::SHC;

class SHCTest : public TransformTest<SHC> {
 public:
  static constexpr int Size = 5000;

  virtual void SetUp() {
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = fabsf(sinf(i * M_PI / 200));
    }
  }
};

TEST_F(SHCTest, Do) {
  SetUseSimd(false);
  Do((*Input)[0], (*Output)[0]);
  ExtremumPoint* results;
  size_t rcount;
  detect_peaks(true, (*Output)[0], output_format_->Size(), kExtremumTypeMaximum,
               &results, &rcount);
  EXPECT_EQ(17, results[0].position);
  EXPECT_NEAR(1, results[0].value, 0.0001f);
  EXPECT_EQ(56, results[1].position);
  EXPECT_NEAR(0.6133f, results[1].value, 0.0001f);
  EXPECT_EQ(90, results[2].position);
  EXPECT_NEAR(0.6133f, results[2].value, 0.0001f);
  SetUseSimd(true);
  Do((*Input)[0], (*Output)[0]);
  detect_peaks(true, (*Output)[0], output_format_->Size(), kExtremumTypeMaximum,
               &results, &rcount);
  EXPECT_EQ(17, results[0].position);
  EXPECT_NEAR(1, results[0].value, 0.0001f);
  EXPECT_EQ(56, results[1].position);
  EXPECT_NEAR(0.6133f, results[1].value, 0.0001f);
  EXPECT_EQ(90, results[2].position);
  EXPECT_NEAR(0.6133f, results[2].value, 0.0001f);
}
