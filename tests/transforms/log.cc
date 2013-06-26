/*! @file log.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Log.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <math.h>
#include "src/transforms/log.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::RawFormatF;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::LogRaw;

class LogTest : public TransformTest<LogRaw> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 378;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = (i + Size / 2.0f) / Size;
    }
  }
};

#define EPSILON 0.000075f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(LogTest, Do) {
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    float log = (*Output)[0][i];
    float vlog = logf((i + Size / 2.0f) / Size);
    ASSERT_EQF(vlog, log);
  }
}

#define CLASS_NAME LogTest
#include "tests/transforms/benchmark.inc"

