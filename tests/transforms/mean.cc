/*! @file complex_to_real.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Mean.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include <math.h>
#include <gtest/gtest.h>
#include "src/transforms/mean.h"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::Formats::FixedArray;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Mean;

class MeanTest
    : public Mean, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<FixedArray<SoundFeatureExtraction::Transforms::MEAN_TYPE_COUNT>> Output;
  int Size;

  MeanTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 486;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      Input[0].Data.get()[i] = i + 1;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    SetParameter("types", "arithmetic geometric");
    InitializeBuffers(Input, &Output);
  }
};

#define EPSILON 0.15f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(MeanTest, Do) {
  Do(Input[0], &Output[0]);

  float amean =0.f;
  float gmean = 1.f, tmp = 1.f;
  for (int j = 0; j < Size; j++) {
    float val = Input[0].Data.get()[j];
    amean += val;
    float multmp = tmp * val;
    if (multmp == std::numeric_limits<float>::infinity()) {
      gmean *= powf(tmp, 1.f / Size);
      tmp = val;
    } else {
      tmp = multmp;
    }
  }
  amean /= Size;
  gmean *= powf(tmp, 1.f / Size);
  ASSERT_EQF(amean, (Output[0])
      [SoundFeatureExtraction::Transforms::MEAN_TYPE_ARITHMETIC]);
  ASSERT_EQF(gmean, (Output[0])
      [SoundFeatureExtraction::Transforms::MEAN_TYPE_GEOMETRIC]);
  ASSERT_EQF(amean,
             Do(false, Input[0].Data.get(), Size,
                SoundFeatureExtraction::Transforms::MEAN_TYPE_ARITHMETIC));
  ASSERT_EQF(gmean,
             Do(false, Input[0].Data.get(), Size,
                SoundFeatureExtraction::Transforms::MEAN_TYPE_GEOMETRIC));
}

#define EXTRA_PARAM SoundFeatureExtraction::Transforms::MEAN_TYPE_ARITHMETIC
#define CLASS_NAME MeanTest
#define ITER_COUNT 500000
#define NO_OUTPUT
#define BENCH_NAME BenchmarkArithmetic
#include "tests/transforms/benchmark.inc"

#undef EXTRA_PARAM
#define EXTRA_PARAM SoundFeatureExtraction::Transforms::MEAN_TYPE_GEOMETRIC
#undef BENCH_NAME
#define BENCH_NAME BenchmarkGeometric
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
