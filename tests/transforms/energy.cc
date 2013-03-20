/*! @file energy.cc
 *  @brief Tests for SpeechFeatureExtraction::Transforms::Energy.
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
#include "src/transforms/energy.h"

using namespace SpeechFeatureExtraction::Formats;
using SpeechFeatureExtraction::BuffersBase;
using SpeechFeatureExtraction::Transforms::Energy;

class EnergyTest
    : public Energy, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  virtual void SetUp() {
    Size = 378;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      Input[0]->Data.get()[i] = i / 40.0f;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 18000, 18000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

#define EPSILON 0.0001f

#define ASSERT_EQF(a, b) do { \
  ASSERT_GT(a + EPSILON, b); \
  ASSERT_LT(a - EPSILON, b); \
} while (0)

TEST_F(EnergyTest, Do) {
  Do(Input, &Output);
  for (int i = 0; i < Size / 2; i++) {
    float m = Output[0]->Data.get()[i];
    float re = i * 2;
    float im = i * 2 + 1;
    ASSERT_EQF((re * re + im * im) / 1600.0f, m);
  }
}

#include "tests/google/src/gtest_main.cc"
