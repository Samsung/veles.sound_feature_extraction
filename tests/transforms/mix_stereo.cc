/*! @file centroid.cc
 *  @brief Tests for sound_feature_extraction::transforms::Centroid.
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
#include "src/transforms/mix_stereo.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormat16;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::MixStereo;

class MixStereoTest : public TransformTest<MixStereo> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 450;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i += 2) {
      (*Input)[0][i] = i;
      (*Input)[0][i + 1] = -i * 2;
    }
  }
};

TEST_F(MixStereoTest, Do) {
  ASSERT_EQ(Size / 2, output_format_->Size());
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size / 2; i++) {
    ASSERT_EQ(-i, (*Output)[0][i]);
  }
}
