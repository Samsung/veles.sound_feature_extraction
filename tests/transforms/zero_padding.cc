/*! @file zero_padding.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::ZeroPadding.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/zero_padding.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::ArrayFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::ZeroPadding;

class ZeroPaddingTest : public TransformTest<ZeroPadding> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 501;
    SetUpTransform(1, Size, 16000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = (i - Size / 2.0f) / Size;
    }
  }
};

TEST_F(ZeroPaddingTest, Do) {
  ASSERT_EQ(512U, outputFormat_->Size());
  Do((*Input)[0], (*Output)[0]);
  for (int i = Size; i < 512; i++) {
    ASSERT_EQ(0.f, (*Output)[0][i]);
  }
}
