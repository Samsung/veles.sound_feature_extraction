/*! @file subsampling.cc
 *  @brief Tests for sound_feature_extraction::transforms::Subsampling.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include "src/transforms/subsampling.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Subsampling;

class SubsamplingTest : public TransformTest<Subsampling> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 600;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i;
    }
  }
};

TEST_F(SubsamplingTest, Do) {
  set_factor(2);
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size / 2; i++) {
    ASSERT_FLOAT_EQ((*Input)[0][i * 2], (*Output)[0][i]) << i;
  }
}
