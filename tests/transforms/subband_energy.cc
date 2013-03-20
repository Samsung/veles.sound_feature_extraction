/*! @file subband_energy.cc
 *  @brief New file description.
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
#include "src/transforms/subband_energy.h"

using SpeechFeatureExtraction::Formats::WindowF;
using SpeechFeatureExtraction::Formats::WindowFormatF;
using SpeechFeatureExtraction::BuffersBase;
using SpeechFeatureExtraction::Transforms::SubbandEnergy;

class SubbandEnergyTest : public SubbandEnergy, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  virtual void SetUp() {
    SetParameter("tree", "3 3 2 2 3 3");
    Size = 512;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      Input[0]->Data.get()[i] = i + 1;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 16000, 16000);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
    Initialize();
  }
};

#define EPSILON 0.005f

#define ASSERT_EQF(a, b) do { \
  ASSERT_GT(a + EPSILON, b); \
  ASSERT_LT(a - EPSILON, b); \
} while (0)

float SumOfSquares(int max) {
  return max * (max + 1) * (2 * max + 1.0f) / 6;
}

TEST_F(SubbandEnergyTest, Do) {
  ASSERT_EQ(6, outputFormat_->Size());
  Do(Input, &Output);
  float* output = Output[0]->Data.get();
  int quarter = Size / 8;
  ASSERT_EQF(SumOfSquares(quarter) / quarter, output[0]);
  ASSERT_EQF((SumOfSquares(quarter * 2) - SumOfSquares(quarter)) / quarter,
             output[1]);
  ASSERT_EQF(
      (SumOfSquares(quarter * 4) - SumOfSquares(quarter * 2)) / (2 * quarter),
      output[2]);
}

#include "tests/google/src/gtest_main.cc"
