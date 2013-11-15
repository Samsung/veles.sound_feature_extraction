/*! @file rolloff.cc
 *  @brief Tests for sound_feature_extraction::transforms::Rolloff.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <cmath>
#include "src/transforms/rolloff.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Rolloff;

class RolloffTest : public TransformTest<Rolloff> {
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

#define EPSILON 0.0025f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(RolloffTest, Do) {
  set_ratio(0.8);
  Do((*Input)[0], &(*Output)[0]);
  float res = Do(false, (*Input)[0], Size, 0.8f);
  res /= input_format_->Duration();
  ASSERT_EQF(res, (*Output)[0]);
}

#define CLASS_NAME RolloffTest
#define ITER_COUNT 400000
#define NO_OUTPUT
#define EXTRA_PARAM 0.85f
#include "tests/transforms/benchmark.inc"
