/*! @file real_to_complex.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::RealToComplex.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/real_to_complex.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::ArrayFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::RealToComplex;

class RealToComplexTest : public TransformTest<RealToComplex> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 486;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i;
    }
  }
};

TEST_F(RealToComplexTest, Do) {
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    ASSERT_EQ(i, (*Output)[0][i * 2]);
  }
}

#define CLASS_NAME RealToComplexTest
#define ITER_COUNT 500000
#include "tests/transforms/benchmark.inc"
