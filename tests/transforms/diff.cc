/*! @file diffrect.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Diff.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include "src/transforms/diff.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::RawFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Diff;

class DiffTest : public TransformTest<Diff> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 486;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i % 3 - i % 2;
    }
    SetParameter("swt", "1");
    Initialize();
  }
};

TEST_F(DiffTest, Do) {
  SetParameter("rectify", "true");
  SetParameter("swt", "0");
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size - 1; i++) {
    int result;
    switch (i % 6) {
      case 1:
      case 3:
        result = 2;
        break;
      default:
        result = 0;
        break;
    }
    ASSERT_FLOAT_EQ(result, (*Output)[0][i]) << i;
  }
  ASSERT_FLOAT_EQ(0, (*Output)[0][Size - 1]);
}

TEST_F(DiffTest, DoNoRectify) {
  SetParameter("rectify", "false");
  SetParameter("swt", "0");
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size - 1; i++) {
    int result;
    switch (i % 6) {
      case 1:
      case 3:
        result = 2;
        break;
      case 2:
        result = -3;
        break;
      case 5:
        result = -1;
        break;
      default:
        result = 0;
        break;
    }
    ASSERT_FLOAT_EQ(result, (*Output)[0][i]) << i;
  }
  ASSERT_FLOAT_EQ(-1, (*Output)[0][Size - 1]);
}

TEST_F(DiffTest, DoSWT) {
  SetParameter("rectify", "true");
  SetParameter("swt", "1");
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    ASSERT_GE((*Output)[0][i], 0);
  }
  SetParameter("swt", "2");
  SetParameter("rectify", "false");
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    ASSERT_EQ((*Output)[0][i], (*Output)[0][i]);
  }
}

#define CLASS_NAME DiffTest
#define ITER_COUNT 500000
#include "tests/transforms/benchmark.inc"
