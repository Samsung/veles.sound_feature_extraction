/*! @file zerocrossings.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::ZeroCrossings.
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
#include "src/transforms/zerocrossings.h"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::ZeroCrossings;

class ZeroCrossingsTest : public ZeroCrossings, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<int32_t> Output;
  int Size;

  virtual void SetUp() {
    Size = 450;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      // Always liked exotic functions
      Input[0]->Data.get()[i] = sinf(i * M_PI / 2);
    }
    Input[0]->Data.get()[Size - 1] = 0;
    Input[0]->Data.get()[Size - 2] = 0;
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(ZeroCrossingsTest, Do) {
  Do(Input, &Output);
  ASSERT_EQ(Size / 2 + 1, *Output[0]);
}

#define CLASS_NAME ZeroCrossingsTest
#define ITER_COUNT 400000
#define NO_OUTPUT
#include "src/primitives/energy.h"
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
