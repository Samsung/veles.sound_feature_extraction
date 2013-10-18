/*! @file complex_magnitude.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::ComplexMagnitude.
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
#include "src/transforms/complex_magnitude.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::ArrayFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::ComplexMagnitude;

class ComplexMagnitudeTest : public TransformTest<ComplexMagnitude> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 378;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i;
    }
  }
};

#define EPSILON 0.000075f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(ComplexMagnitudeTest, Do) {
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size / 2; i++) {
    float m = (*Output)[0][i];
    float re = i * 2;
    float im = i * 2 + 1;
    ASSERT_EQF(sqrtf(re * re + im * im), m);
  }
}

#define CLASS_NAME ComplexMagnitudeTest
#include "tests/transforms/benchmark.inc"
