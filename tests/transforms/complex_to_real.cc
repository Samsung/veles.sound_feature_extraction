/*! @file complex_to_real.cc
 *  @brief Tests for sound_feature_extraction::transforms::ComplexToReal.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include "src/transforms/complex_to_real.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::ComplexToReal;

class ComplexToRealTest : public TransformTest<ComplexToReal> {
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

TEST_F(ComplexToRealTest, Do) {
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size / 2; i++) {
    int result = 2 * i;
    ASSERT_EQ(result, (*Output)[0][i]);
  }
}

#define CLASS_NAME ComplexToRealTest
#define ITER_COUNT 800000
#include "tests/transforms/benchmark.inc"
