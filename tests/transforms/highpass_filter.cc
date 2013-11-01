/*! @file highpass_filter.cc
 *  @brief Tests for sound_feature_extraction::transforms::HighpassFilter.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <fstream>
#include "src/transforms/highpass_filter.h"
#include "tests/transforms/transform_test.h"
#include "src/primitives/window.h"
#include "tests/transforms/pass_signal.inc"

#define EPSILON 0.0001f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

using sound_feature_extraction::transforms::HighpassFilter;

class HighpassFilterTest : public TransformTest<HighpassFilter> {
 public:
  virtual void SetUp() {
    set_frequency(2000);
    Size = sizeof(data_signal) / sizeof(data_signal[0]);
    SetUpTransform(1, 10 * Size, 22050);
    for (int i = 0; i < 10; i++) {
      memcpy((*Input)[0] + i * Size, data_signal, sizeof(data_signal));
    }
  }

  size_t Size;
};

TEST_F(HighpassFilterTest, CalculateHighpassFilter) {
  Do((*Input)[0], (*Output)[0]);

  std::ofstream fs("/tmp/out.txt", std::ios::out);
  fs << '[';
  for (int i = 0; i < 10000; i++) {
    fs << (*Output)[0][i] << ", ";
  }
  fs << "]\n";

}
