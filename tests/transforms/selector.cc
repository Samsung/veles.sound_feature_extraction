/*! @file selector.cc
 *  @brief Tests for sound_feature_extraction::transforms::Selector.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/selector.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Selector;

class SelectorTest : public TransformTest<Selector> {
 public:
  int Size;

  virtual void SetUp() {
    set_from(sound_feature_extraction::transforms::kAnchorLeft);
    set_length(6);
    Size = 512;
    SetUpTransform(1, Size, 16000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i;
    }
  }
};

TEST_F(SelectorTest, Do) {
  ASSERT_EQ(6U, output_format_->Size());
  Do((*Input)[0], (*Output)[0]);
  ASSERT_EQ(0, memcmp((*Input)[0],
                      (*Output)[0],
                      6 * sizeof(float)));  // NOLINT(*)
  set_from(sound_feature_extraction::transforms::kAnchorRight);
  Do((*Input)[0], (*Output)[0]);
  ASSERT_EQ(0, memcmp((*Input)[0] + 512 - 6,
                      (*Output)[0],
                      6 * sizeof(float)));  // NOLINT(*)
}
