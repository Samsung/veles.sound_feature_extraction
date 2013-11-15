/*! @file peak_detection.cc
 *  @brief Tests for sound_feature_extraction::transforms::PeakDetection.
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

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::PeakDetection;

class PeakDetectionTest : public TransformTest<PeakDetection> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 608;
    set_swt_level(4);
    set_swt_order(4);
    set_number(3);
    set_sort(sound_feature_extraction::kSortOrderBoth);
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = sinf(i * M_PI / 100) *
          (1.f + 0.05 * (0.5f - (rand() + 0.f) / RAND_MAX));
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
