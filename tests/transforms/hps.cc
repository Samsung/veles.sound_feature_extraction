/*! @file hps.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::HPS.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <math.h>
#include "src/transforms/hps.h"
#include "tests/transforms/transform_test.h"
#include "tests/transforms/hps_test.inc"

using SoundFeatureExtraction::Formats::ArrayFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Hps;

class HpsTest : public TransformTest<Hps> {
 public:
  int Size;

  virtual void SetUp() {
    Size = data[1];
    SetUpTransform(1, Size, data[0]);
    for (int i = 0; i < data[1]; ++i) {
      (*Input)[0][i] = data[i + 2];
    }
  }
};

TEST_F(HpsTest, Do) {
  Do((*Input)[0], &(*Output)[0]);
  float delta = (*Output)[0] - 110.671507f;
  ASSERT_LT(delta * delta, 1.0f);
}
