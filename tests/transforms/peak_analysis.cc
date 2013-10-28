/*! @file peak_analysis.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::PeakAnalysis.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include "src/transforms/peak_analysis.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::ArrayFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::PeakAnalysis;

class PeakAnalysisTest : public TransformTest<PeakAnalysis> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 4;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i][0] = (i + 1) * 10;
      (*Input)[0][i][1] = 100;
    }
  }
};

TEST_F(PeakAnalysisTest, Do) {
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    if (i == 0) {
      ASSERT_FLOAT_EQ(10, (*Output)[0][i * 2]);
    } else {
      ASSERT_FLOAT_EQ(i + 1, (*Output)[0][i * 2]);
    }
    ASSERT_FLOAT_EQ(0.25f, (*Output)[0][i * 2 + 1]);
  }
}
