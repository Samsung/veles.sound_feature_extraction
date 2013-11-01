/*! @file rectify.cc
 *  @brief Tests for sound_feature_extraction::transforms::RECTIFY.
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
#include "src/transforms/rectify.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Rectify;

class RectifyTest : public TransformTest<Rectify> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 486;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; ++i) {
      (*Input)[0][i] = i;
      if (i % 2 == 0) {
        (*Input)[0][i] *= -1;
      }
    }
  }
};

TEST_F(RectifyTest, Do) {
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    ASSERT_EQ(i, (*Output)[0][i]);
  }
}

#define CLASS_NAME RectifyTest
#define ITER_COUNT 500000
#include "tests/transforms/benchmark.inc"
