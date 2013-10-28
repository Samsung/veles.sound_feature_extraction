/*! @file peak_detection.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::PeakDetection.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include "src/transforms/peak_detection.h"
#include <cmath>
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::ArrayFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::PeakDetection;

class PeakDetectionTest : public TransformTest<PeakDetection> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 608;
    SetParameter("swt_level", "4");
    SetParameter("swt_order", "4");
    SetParameter("number", "3");
    SetParameter("sort", "both");
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = sinf(i * M_PI / 100) *
          (1.f + 0.1 * (0.5f - (rand() + 0.f) / RAND_MAX));
    }
  }
};

TEST_F(PeakDetectionTest, Do) {
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < 3; i++) {
    EXPECT_NEAR((*Output)[0][i][0] * Size, 50.f + i * 200 - 10, 5);
    EXPECT_NEAR((*Output)[0][i][1], 1, 0.05f) << i;
  }
}
