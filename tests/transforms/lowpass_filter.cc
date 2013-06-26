/*! @file lowpass_filter.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::LowpassFilter.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/lowpass_filter.h"
#include "tests/transforms/transform_test.h"
#include "src/primitives/window.h"

#define EPSILON 0.0001f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

using SoundFeatureExtraction::Transforms::LowpassFilter;

class LowpassFilterTest : public TransformTest<LowpassFilter> {
 public:
  virtual void SetUp() {
    SetParameter("frequency", "600");
  }
};

TEST_F(LowpassFilterTest, CalculateLowpassFilter) {
  length_ = 4;
  inputFormat_->SetSamplingRate(16000);
  std::vector<float> filter(length_);
  for (size_t i = 0; i < filter.size(); i++) {
    filter[i] = WindowElement(WINDOW_TYPE_HAMMING, length_, i);
  }
  CalculateFilter(&filter[0]);
  ASSERT_EQF(0.0059f, filter[0]);
  ASSERT_EQF(0.0576f, filter[1]);
  ASSERT_EQF(0.0576f, filter[2]);
  ASSERT_EQF(0.0059f, filter[3]);

  length_ = 5;
  filter.resize(length_);
  for (size_t i = 0; i < filter.size(); i++) {
    filter[i] = WindowElement(WINDOW_TYPE_HAMMING, length_, i);
  }
  CalculateFilter(&filter[0]);
  ASSERT_EQF(0.0058f, filter[0]);
  ASSERT_EQF(0.0401f, filter[1]);
  ASSERT_EQF(0.0750f, filter[2]);
  ASSERT_EQF(0.0401f, filter[3]);
  ASSERT_EQF(0.0058f, filter[4]);
}
