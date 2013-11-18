/*! @file delta.cc
 *  @brief Tests for sound_feature_extraction::transforms::Delta.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include "src/transforms/delta.h"
#include "tests/transforms/transform_test.h"
#include <fftf/api.h>

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Delta;

class DeltaTest : public TransformTest<Delta> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 486;
    SetUpTransform(2, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i;
      (*Input)[1][i] = i + 1;
    }
  }
};

TEST_F(DeltaTest, Do) {
  ASSERT_EQ(2U, (*Output).Count());
  Do((*Input), &(*Output));
  for (int i = 0; i < Size; i++) {
    ASSERT_NEAR((*Output)[0][i], 1, 0.00001f) << i;
  }
}
/*
TEST_F(DeltaTest, DoRegression) {
  set_type(sound_feature_extraction::transforms::kDeltaTypeRegression);
  set_use_simd(false);
  Do((*Input), &(*Output));
  set_use_simd(true);
  auto output_simd = std::static_pointer_cast<Delta::OutBuffers>(
      CreateOutputBuffers(Output->Count()));
  Do((*Input), &(*output_simd));
  for (int i = 0; i < Size; i++) {
    ASSERT_NEAR((*Output)[0][i], (*output_simd)[0][i], 0.00001f) << i;
  }
}
*/
