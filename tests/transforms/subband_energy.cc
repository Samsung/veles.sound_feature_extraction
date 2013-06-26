/*! @file subband_energy.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::SubbandEnergy.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/subband_energy.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::RawFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::SubbandEnergy;

class SubbandEnergyTest : public TransformTest<SubbandEnergy> {
 public:
  int Size;

  virtual void SetUp() {
    SetParameter("tree", "3 3 2 2 3 3");
    Size = 512;
    SetUpTransform(1, Size, 16000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i + 1;
    }
  }
};

#define EPSILON 0.005f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

float SumOfSquares(int max) {
  return max * (max + 1) * (2 * max + 1.0f) / 6;
}

TEST_F(SubbandEnergyTest, Do) {
  ASSERT_EQ(6, outputFormat_->Size());
  Do((*Input)[0], (*Output)[0]);
  float* output = (*Output)[0];
  int quarter = Size / 8;
  ASSERT_EQF(SumOfSquares(quarter) / quarter, output[0]);
  ASSERT_EQF((SumOfSquares(quarter * 2) - SumOfSquares(quarter)) / quarter,
             output[1]);
  ASSERT_EQF(
      (SumOfSquares(quarter * 4) - SumOfSquares(quarter * 2)) / (2 * quarter),
      output[2]);
}
