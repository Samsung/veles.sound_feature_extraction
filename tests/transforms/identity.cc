/*! @file identity.cc
 *  @brief Tests for sound_feature_extraction::transforms::Identity.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include "src/transforms/identity.h"
#include "src/formats/array_format.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Identity;

class IdentityTest : public Identity,
                     public testing::Test {
 public:
  virtual void SetUp() {
    SetInputFormat(std::make_shared<ArrayFormatF>(1000, 22050), 3);
    Input = std::make_shared<BuffersBase<float*>>(
        std::static_pointer_cast<ArrayFormatF>(input_format_), 3);
    Output = std::static_pointer_cast<BuffersBase<float*>>(
        CreateOutputBuffers(3));
    Initialize();

    for (int j = 0; j < 3; j++) {
      for (int i = 0; i < 1000; i++) {
        (*Input)[j][i] = i;
      }
    }
  }

 protected:
  std::shared_ptr<BuffersBase<float*>> Input;
  std::shared_ptr<BuffersBase<float*>> Output;
};

TEST_F(IdentityTest, Do) {
  Do((*Input), Output.get());
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 1000; i++) {
      ASSERT_FLOAT_EQ(i, (*Output)[j][i]);
    }
  }
}
