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
#include "src/transforms/centroid.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Centroid;

class CentroidTest : public TransformTest<Centroid> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 450;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      // Always liked exotic functions
      (*Input)[0][i] = fabs(sinf(i * i) + i * cosf(i));
    }
  }
};

#define EPSILON 0.005f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(CentroidTest, Do) {
  Do((*Input)[0], &(*Output)[0]);
  double res = Do(false, (*Input)[0], Size);
  res /= input_format_->Duration();
  ASSERT_EQF(res, (*Output)[0]);
}

#define CLASS_NAME CentroidTest
#define ITER_COUNT 400000
#define NO_OUTPUT
#include "tests/transforms/benchmark.inc"
