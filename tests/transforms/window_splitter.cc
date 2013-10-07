/*! @file window_splitter.cc
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

#include "tests/transforms/transform_test.h"
#include "src/transforms/window_splitter.h"

using SoundFeatureExtraction::Transforms::WindowSplitterF;

class WindowSplitterTest : public TransformTest<WindowSplitterF> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 512 + 205;
    SetUpTransform(10, Size, 16000);
    for (int j = 0; j < 10; j++) {
      for (int i = 0; i < Size; i++) {
        (*Input)[j][i] = i * (1 - 2 * (i % 2));
      }
    }
  }
};

TEST_F(WindowSplitterTest, Do) {
  Do(*Input, Output.get());
  ASSERT_EQ(Input->Count() * 2, Output->Count());
  ASSERT_EQ(512U, outputFormat_->Size());
}

TEST_F(WindowSplitterTest, DoInverseInterleaved) {
  SetParameter("inverse", "true");
  SetParameter("interleaved", "true");
  SetParameter("step", "309");
  SetParameter("inverse_count", "2");
  RecreateOutputBuffers();
  ASSERT_EQ(2U, Output->Count());
  ASSERT_EQ(1953U, outputFormat_->Size());
  DoInverse(*Input, Output.get());
}
