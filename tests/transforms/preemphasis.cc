/*! @file preemphasis.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Preemphasis.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <gtest/gtest.h>
#include <math.h>
#include "src/transforms/preemphasis.h"

using SoundFeatureExtraction::Formats::Raw16;
using SoundFeatureExtraction::Formats::RawFormat16;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Preemphasis;

class PreemphasisTest : public Preemphasis, public testing::Test {
 public:
  BuffersBase<Raw16> Input;
  BuffersBase<Raw16> Output;
  int Size;

  PreemphasisTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 1024;
    Input.Initialize(1, Size, 2);
    for (int i = 0; i < Size; i++) {
      Input[0][i] = i + 1;
    }
    auto format = std::make_shared<RawFormat16>(Size, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(PreemphasisTest, Do) {
  Do(Input[0], &Output[0]);
  ASSERT_EQ(1, Output[0][0]);
  for (int i = 1; i < Size; i++) {
    ASSERT_NEAR(int16_t((1 - kDefaultK) * (i + 1) + kDefaultK),
                Output[0][i], 1.f);
  }
}

#undef CLASS_NAME
#define CLASS_NAME PreemphasisTest
#undef INPUT_TYPE
#define INPUT_TYPE int16_t
#define PARAM_REORDER
#define EXTRA_PARAM 0.5f
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
