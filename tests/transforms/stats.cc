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


#include <random>
#include "src/transforms/stats.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::SingleFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Stats;

class StatsTest : public TransformTest<Stats> {
 public:
  virtual void SetUp() {
    SetUpTransform(1, 50000, 16000);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> d(1, 2);
    for (size_t i = 0; i < (*Input).Count(); i++) {
      for (size_t j = 0; j < inputFormat_->Size(); j++) {
        (*Input)[i][j] = d(gen);
      }
    }
  }
};

#define EPSILON 0.1f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(StatsTest, DoAll) {
  Do((*Input)[0], (*Output)[0]);
  ASSERT_EQF(1, (*Output)[0][0][0]);
  ASSERT_EQF(2 * 2, (*Output)[0][0][1]);
  ASSERT_EQF(0, (*Output)[0][0][2]);
  ASSERT_EQF(0, (*Output)[0][0][3]);
}

TEST_F(StatsTest, DoInterval) {
  SetParameter("interval", "25000");
  RecreateOutputBuffers();
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < 2; i++) {
    ASSERT_EQF(1, (*Output)[0][i][0]);
    ASSERT_EQF(2 * 2, (*Output)[0][i][1]);
    ASSERT_EQF(0, (*Output)[0][i][2]);
    ASSERT_EQF(0, (*Output)[0][i][3]);
  }
}
