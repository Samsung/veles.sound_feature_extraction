/*! @file beat.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::BEAT.
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
#include "src/transforms/beat.h"
#include "tests/transforms/transform_test.h"
#include "tests/transforms/beat_test.inc"

using SoundFeatureExtraction::Formats::RawFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Beat;

class BeatTest : public TransformTest<Beat<RawFormatF>> {
 public:
  int Size;

  virtual void SetUp() {
    Size = data[0];
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < data[0]; ++i) {
      (*Input)[0][i] = data[i + 1];
    }
  }
};

TEST_F(BeatTest, Do) {
  Do((*Input)[0], &(*Output)[0]);
  float delta = 122.85f - (*Output)[0];
  if (delta * delta > 1E-1) {
    ASSERT_EQ(-1, (*Output)[0]);
  }
}
