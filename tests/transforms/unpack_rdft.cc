/*! @file unpack_rdft.cc
 *  @brief Tests for SpeechFeatureExtraction::Transforms::UnpackRDFT.
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
#include "src/transforms/unpack_rdft.h"

using SpeechFeatureExtraction::Formats::WindowF;
using SpeechFeatureExtraction::Formats::WindowFormatF;
using SpeechFeatureExtraction::BuffersBase;
using SpeechFeatureExtraction::Transforms::UnpackRDFT;

class UnpackRDFTTest : public UnpackRDFT, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  void Initialize(int size) {
    Size = size;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      Input[0]->Data.get()[i] = i;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 2000 / 16000, 16000);
    format->SetSize(Size);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(UnpackRDFTTest, DoReal) {
  Initialize(257);
  Do(Input, &Output);
  for (int i = Size; i < (Size - 1) * 2; i++) {
    ASSERT_EQ(2 * (Size - 1) - i, Output[0]->Data.get()[i]);
  }
}

TEST_F(UnpackRDFTTest, DoComplex) {
  Initialize(258);
  Do(Input, &Output);
  for (int i = Size; i < (Size - 2) * 2; i += 2) {
    ASSERT_EQ(2 * (Size - 2) - i, Output[0]->Data.get()[i]);
    ASSERT_EQ(2 * (Size - 2) - i + 1, Output[0]->Data.get()[i + 1]);
  }
}

#include "tests/google/src/gtest_main.cc"
