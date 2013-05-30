/*! @file rdft.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::RDFT.
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
#include "src/transform_tree.h"
#include "src/formats/raw_format.h"
#include "src/transforms/rdft.h"
#include "tests/speech_sample.inc"

using SoundFeatureExtraction::Formats::WindowF;
using SoundFeatureExtraction::Formats::WindowFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::RDFT;
using SoundFeatureExtraction::TransformTree;
using SoundFeatureExtraction::Formats::Raw16;
using SoundFeatureExtraction::BuffersBase;

class RDFTTest : public RDFT, public testing::Test {
 public:
  BuffersBase<WindowF> Input;
  BuffersBase<WindowF> Output;
  int Size;

  RDFTTest()
      : Input(inputFormat_),
        Output(outputFormat_) {
  }

  virtual void SetUp() {
    Size = 512;
    Input.Initialize(1, Size + 2);
    for (int i = 0; i < Size; i++) {
      Input[0][i] = (i - Size / 2.0f) / Size;
    }
    auto format = std::make_shared<WindowFormatF>(Size * 1000 / 16000, 16000);
    format->SetAllocatedSize(format->Size() + 2);
    SetInputFormat(format);
    InitializeBuffers(Input, &Output);
  }
};

TEST_F(RDFTTest, Forward) {
  Do(Input, &Output);
}

TEST_F(RDFTTest, Backward) {
  SetParameter("inverse", "true");
  outputFormat_->SetSize(Size);
  Do(Input, &Output);
}

TEST(RDFT, Multiple) {
  TransformTree tt( { 48000, 16000 } );  // NOLINT(*)
  tt.SetValidateAfterEachTransform(true);
  // We have to apply FilterBank twice since Energy results in
  // squared magnitude
  tt.AddFeature("3RDFT", { { "Window", "length=512, type=rectangular" },
      { "RDFT", "" }, { "IRDFT", "" }, { "RDFT", "" }, { "IRDFT", "" },
      { "RDFT", "" }, { "IRDFT", "" }
  });
  Raw16 buffers(48000, 0);
  memcpy(buffers.Data.get(), data, sizeof(data));
  tt.PrepareForExecution();
  auto res = tt.Execute(buffers);
  ASSERT_EQ(1, res.size());
  res["3RDFT"]->Validate();
}

#include "tests/google/src/gtest_main.cc"

