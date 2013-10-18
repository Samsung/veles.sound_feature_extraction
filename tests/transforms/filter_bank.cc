/*! @file filter_bank.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::FilterBank.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */



#include "src/transforms/filter_bank.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::ArrayFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::FilterBank;

class FilterBankTest : public TransformTest<FilterBank> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 378;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = 100;
    }
  }
};

#define EPSILON 0.000075f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(FilterBankTest, Do) {
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    ASSERT_EQF(100 * filterBank_.get()[i], (*Output)[0][i]);
  }
}
