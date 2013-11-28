/*! @file reorder.cc
 *  @brief Tests for sound_feature_extraction::transforms::Reorder.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/reorder.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Reorder;

class ReorderTest : public TransformTest<Reorder> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 122;
    SetUpTransform(1, Size, 16000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i;
    }
  }
};

TEST_F(ReorderTest, Do) {
  ASSERT_EQ(120U, output_format_->Size());
  Do((*Input)[0], (*Output)[0]);
  for (int i = Size; i < 120; i++) {
    ASSERT_EQ(i / 10 + 12 * (i % 10), (*Output)[0][i]);
  }
}
