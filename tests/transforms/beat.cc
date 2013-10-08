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

using SoundFeatureExtraction::Transforms::Beat;

class BeatTest : public TransformTest<Beat> {
 public:
  int Size;

  virtual void SetUp() {
    Size = sizeof(data_test) / sizeof(data_test[0]);
    SetUpTransform(1, Size, 3000);
    memcpy((*Input)[0], data_test, Size * sizeof(float));
  }
};

TEST_F(BeatTest, Do) {
  Do((*Input), &(*Output));
  ASSERT_NEAR(180.f, (*Output)[0], 0.2f);
}

TEST_F(BeatTest, CombConvolve) {
  auto data_size = sizeof(data_conv) / sizeof(data_conv[0]);
  float out[data_size + 2001 - 1];
  CombConvolve(data_conv, data_size, 3, 1000, out);
  for (int i = 0; i < static_cast<int>(data_size) + 2000; i++) {
    ASSERT_NEAR(out[i], data_conv_result[i], 0.0001f) << "i = " << i;
  }
}
