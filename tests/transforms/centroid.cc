/*! @file centroid.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Centroid.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <gtest/gtest.h>
#include <math.h>
#include "src/transforms/centroid.h"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Centroid;

class CentroidTest : public Centroid, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<float> Output;
  int Size;

  virtual void SetUp() {
    Size = 450;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      // Always liked exotic functions
      Input[0]->Data.get()[i] = fabs(sinf(i * i) + i * cosf(i));
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

#define EPSILON 0.0025f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(CentroidTest, Do) {
  Do(Input, &Output);
  double res = Do(false, Input[0]->Data.get(), Size);
  res *= inputFormat_->SamplingRate() / (2.f * inputFormat_->SamplesCount());
  ASSERT_EQF(res, *Output[0]);
}

#define CLASS_NAME CentroidTest
#define ITER_COUNT 400000
#define NO_OUTPUT
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
