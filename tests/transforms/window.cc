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
#include "src/transform_tree.h"
#include "src/transform_registry.h"
#include "src/formats/raw_format.h"
#include "tests/speech_sample.inc"

using SoundFeatureExtraction::Formats::Raw16;
using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Window;
using SoundFeatureExtraction::TransformTree;

class WindowTest : public Window, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  WindowTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 514;
    Input.Initialize(1, Size);
    for (int i = 0; i < Size; i++) {
      Input[0]->Data.get()[i] = i * (1 - 2 * (i % 2));
    }
    auto format = std::make_shared<WindowFormatF>(Size * 2000 / 16000, 16000);
    format->SetSize(Size);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(WindowTest, Do) {
  SetParameter("predft", "false");
  Initialize();
  Do(Input, &Output);
  float output_na[Size] __attribute__ ((aligned (32)));
  ApplyWindow(false, window_.get(), Size, Input[0]->Data.get(), output_na);
  ASSERT_EQ(0, memcmp(Output[0]->Data.get(), output_na,
                      sizeof(float) * Size));
}

TEST_F(WindowTest, DoPreDft) {
  SetParameter("predft", "true");
  Initialize();
  Do(Input, &Output);
}

TEST(RawWindow, Inverse) {
  Raw16 buffers(48000, 0);
  TransformTree tt( { 48000, 16000 } );  // NOLINT(*)
  tt.SetValidateAfterEachTransform(true);
  // We have to apply FilterBank twice since Energy results in
  // squared magnitude
  tt.AddFeature("WindowInverseTest", { { "Window", "length=512" },
      { "IWindow", "length=512" } });
  memcpy(buffers.Data.get(), data, sizeof(data));
  tt.PrepareForExecution();
  auto res = tt.Execute(buffers);
  ASSERT_EQ(1, res.size());
  res["WindowInverseTest"]->Validate();
  auto report = tt.ExecutionTimeReport();
  for (auto r : report) {
    printf("%s:\t%f\n", r.first.c_str(), r.second);
  }
}

#include "tests/google/src/gtest_main.cc"
