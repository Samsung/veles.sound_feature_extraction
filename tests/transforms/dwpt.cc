/*! @file dwpt.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::DWPT.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/dwpt.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::RawFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::DWPT;

class DWPTTest : public TransformTest<DWPT> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 512;
    SetUpTransform(1, Size, 16000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = (i - Size / 2.0f) / Size;
    }
  }
};

TEST_F(DWPTTest, Forward) {
  ASSERT_EQ(inputFormat_->Size(), outputFormat_->Size());
  Do((*Input)[0], (*Output)[0]);
}

/*
TEST_F(DWPTTest, Backward) {
  SetParameter("inverse", "true");
  Do((*Input), &(*Output));
}
*/

