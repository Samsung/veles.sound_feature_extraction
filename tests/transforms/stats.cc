/*! @file argminmax.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Stats.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include <gtest/gtest.h>
#include <random>
#include "src/transforms/stats.h"

using SoundFeatureExtraction::Formats::SingleFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Stats;

class StatsTest
    : public Stats, public testing::Test {
 public:
  BuffersBase<float> Input;
  BuffersBase<StatsArray> Output;

  StatsTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Input.Initialize(50000);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> d(1, 2);
    for (size_t i = 0; i < Input.Size(); i++) {
      Input[i] = d(gen);
    }
    auto format = std::make_shared<SingleFormatF>();
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

#define EPSILON 0.1f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(StatsTest, Do) {
  Do(Input, &Output);
  ASSERT_EQF(1, Output[0][0]);
  ASSERT_EQF(2 * 2, Output[0][1]);
  ASSERT_EQF(0, Output[0][2]);
  ASSERT_EQF(0, Output[0][3]);
}

#include "tests/google/src/gtest_main.cc"
