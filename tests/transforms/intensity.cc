/*! @file intensity.cc
 *  @brief Tests for sound_feature_extraction::transforms::Intensity.
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
#include "src/transforms/intensity.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Intensity;

class IntensityTest : public TransformTest<Intensity> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 378;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i / 40.0f;
    }
  }
};

#define EPSILON 0.000075f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(IntensityTest, Do) {
  Do((*Input)[0], &(*Output)[0]);
  double res = .0;
  for (int i = 0; i < Size; i++) {
    float tmp = (*Input)[0][i];
    res += tmp * tmp;
  }
  ASSERT_EQF(logf(res / Size), (*Output)[0]);
}

#define CLASS_NAME IntensityTest
#define ITER_COUNT 500000
#define BENCH_FUNC(a, b, c) logf(calculate_energy(a, b, c))
#define NO_OUTPUT
#include "src/primitives/energy.h"
#include "tests/transforms/benchmark.inc"
