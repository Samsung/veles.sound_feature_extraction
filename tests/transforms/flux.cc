/*! @file flux.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Flux.
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
#include "src/transforms/flux.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::RawFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Flux;

class FluxTest : public TransformTest<Flux> {
 public:
  static const int Size;

  virtual void SetUp() {
    SetUpTransform(2, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = fabs(sinf(i * i) + i * cosf(i));
      (*Input)[1][i] = 0.f;
    }
  }
};

const int FluxTest::Size = 450;

#define EPSILON 0.0001f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(FluxTest, Do) {
  Do((*Input), &(*Output));
  float res = Do(false, (*Input)[1], Size, (*Input)[0]);
  ASSERT_EQ(0.f, (*Output)[0]);
  ASSERT_EQF(res, (*Output)[1]);
}

const float extra_param[FluxTest::Size] = { 0.f };

#define CLASS_NAME FluxTest
#define ITER_COUNT 400000
#define NO_OUTPUT
#define EXTRA_PARAM extra_param
#include "tests/transforms/benchmark.inc"
