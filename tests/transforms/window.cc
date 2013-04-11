/*! @file window.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Window.
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
#include "src/transforms/window.h"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Window;

class UnpackRDFTTest : public Window, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  virtual void SetUp() {
    Size = 514;
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

TEST_F(UnpackRDFTTest, Do) {
  SetParameter("predft", "false");
  Initialize();
  Do(Input, &Output);
}

TEST_F(UnpackRDFTTest, DoPreDft) {
  SetParameter("predft", "true");
  Initialize();
  Do(Input, &Output);
}

#include "tests/google/src/gtest_main.cc"
