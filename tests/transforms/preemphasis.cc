/*! @file preemphasis.cc
 *  @brief Tests for sound_feature_extraction::transforms::Preemphasis.
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
#include "src/transforms/preemphasis.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Preemphasis;

class PreemphasisTest : public TransformTest<Preemphasis> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 1024;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i + 1;
    }
  }
};

TEST_F(PreemphasisTest, Do) {
  Do((*Input)[0], (*Output)[0]);
  ASSERT_EQ(1, (*Output)[0][0]);
  for (int i = 1; i < Size; i++) {
    ASSERT_NEAR((1 - kDefaultValue) * (i + 1) + kDefaultValue,
                (*Output)[0][i], 1.f) << i;
  }
}

#undef CLASS_NAME
#define CLASS_NAME PreemphasisTest
#define PARAM_REORDER
#define EXTRA_PARAM 0.5f
#include "tests/transforms/benchmark.inc"
