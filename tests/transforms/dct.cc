/*! @file dct.cc
 *  @brief Tests for SpeechFeatureExtraction::Transforms::DCT.
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
#include "src/transforms/dct.h"

using SpeechFeatureExtraction::Formats::WindowF;
using SpeechFeatureExtraction::Formats::WindowFormatF;
using SpeechFeatureExtraction::BuffersBase;
using SpeechFeatureExtraction::Transforms::DCT;

class DCTTest : public DCT, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  virtual void SetUp() {
    Size = 512;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      Input[0]->Data.get()[i] = (i - Size / 2.0f) / Size;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 16000, 16000);
    SetInputFormat(format);
    TypeSafeInitializeBuffers(Input, &Output);
  }
};

TEST_F(DCTTest, Forward) {
  TypeSafeDo(Input, &Output);
}

TEST_F(DCTTest, Backward) {
  SetParameter("inverse", "true");
  TypeSafeDo(Input, &Output);
}

#include "tests/google/src/gtest_main.cc"