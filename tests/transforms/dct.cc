/*! @file dct.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::DCT.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/dct.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::RawFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::DCT;

class DCTTest : public TransformTest<DCT> {
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

TEST_F(DCTTest, Forward) {
  ASSERT_EQ(inputFormat_->Size(), outputFormat_->Size());
  Do((*Input), &(*Output));
}

TEST_F(DCTTest, Backward) {
  SetParameter("inverse", "true");
  ASSERT_EQ(inputFormat_->Size(), outputFormat_->Size());
  Do((*Input), &(*Output));
}
