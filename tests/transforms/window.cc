/*! @file window.cc
 *  @brief Tests for sound_feature_extraction::transforms::Window.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include "src/transforms/window.h"
#include "tests/transforms/transform_test.h"
#include "src/transform_tree.h"
#include "src/transform_registry.h"
#include "src/formats/array_format.h"
#include "tests/speech_sample.inc"

using sound_feature_extraction::transforms::Window;

class WindowTest : public TransformTest<Window> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 514;
    SetUpTransform(1, Size, 16000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i * (1 - 2 * (i % 2));
    }
  }
};

TEST_F(WindowTest, Do) {
  set_predft(false);
  Initialize();
  Do((*Input)[0], (*Output)[0]);
  float output_na[Size] __attribute__((aligned (32)));
  ApplyWindow(false, window_.get(), Size, (*Input)[0], output_na);
  ASSERT_EQ(0, memcmp((*Output)[0], output_na,
                      sizeof(float) * Size));
}

TEST_F(WindowTest, DoPreDft) {
  set_predft(true);
  Initialize();
  Do((*Input)[0], (*Output)[0]);
}
