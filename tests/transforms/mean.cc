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

  virtual void SetUp() {
    Size = 486;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      Input[0]->Data.get()[i] = i + 1;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    SetParameter("types", "arithmetic geometric");
    InitializeBuffers(Input, &Output);
  }
};

#define EPSILON 0.0001f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(MeanTest, Do) {
  Do(Input, &Output);

  float amean =.0f;
  float gmean = 1.f;
  float tmpg = 1.0f;
  for (int i = 0; i < Size; i++) {
    float val = Input[0]->Data.get()[i];
    amean += val;
    tmpg *= val;
    if (i % 8 == 0) {
      gmean *= powf(tmpg, 1.f / Size);
      tmpg = 1.0f;
    }
  }
  amean /= Size;
  gmean *= powf(tmpg, 1.f / Size);
  ASSERT_EQF(amean, (*Output[0])
      [SoundFeatureExtraction::Transforms::MEAN_TYPE_ARITHMETIC]);
  ASSERT_EQF(gmean, (*Output[0])
      [SoundFeatureExtraction::Transforms::MEAN_TYPE_GEOMETRIC]);
}

#include "tests/google/src/gtest_main.cc"
