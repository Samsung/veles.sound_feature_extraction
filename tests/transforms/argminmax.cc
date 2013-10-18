/*! @file argminmax.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::ArgMinMax.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include "src/transforms/argminmax.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::ArrayFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::ArgMinMax;
using SoundFeatureExtraction::Transforms::ArgMinMaxResult;

class ArgMinMaxTest : public TransformTest<ArgMinMax> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 486;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size / 2; i++) {
      (*Input)[0][i] = -i + 1;
    }
    for (int i = Size / 2; i < Size; i++) {
      (*Input)[0][i] = Size - i + 1;
    }
  }
};

#define EPSILON 0.15f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(ArgMinMaxTest, Do) {
  SetParameter("extremum", "max");
  Do((*Input)[0], &(*Output)[0]);
  ASSERT_EQ(Size / 2, std::get<0>((*Output)[0]));
  ASSERT_EQ(Size / 2 + 1, std::get<1>((*Output)[0]));
  SetParameter("extremum", "min");
  Do((*Input)[0], &(*Output)[0]);
  ASSERT_EQ(Size / 2 - 1, std::get<0>((*Output)[0]));
  ASSERT_EQ(- Size / 2 + 2, std::get<1>((*Output)[0]));
}

#define EXTRA_PARAM true
#define CLASS_NAME ArgMinMaxTest
#define ITER_COUNT 500000
#define NO_OUTPUT
#include "tests/transforms/benchmark.inc"
