/*! @file square.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::SquareRaw.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/square.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::ArrayFormat16;
using SoundFeatureExtraction::Formats::ArrayFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Square;

class SquareTest : public TransformTest<Square> {
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

#define EPSILON 0.01f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(SquareTest, Do) {
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    ASSERT_EQF(i * i, (*Output)[0][i]);
  }
}

#define CLASS_NAME SquareTest
#define ITER_COUNT 300000
#include "tests/transforms/benchmark.inc"
