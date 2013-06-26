/*! @file zerocrossings.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::ZeroCrossings.
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
#include "src/transforms/zerocrossings.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::RawFormatF;
using SoundFeatureExtraction::Formats::RawFormat16;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::ZeroCrossingsF;
using SoundFeatureExtraction::Transforms::ZeroCrossings16;

class ZeroCrossingsWindowTest : public TransformTest<ZeroCrossingsF> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 450;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      // Always liked exotic functions
      (*Input)[0][i] = sinf(i * M_PI / 2);
    }
    (*Input)[0][Size - 1] = 0;
    (*Input)[0][Size - 2] = 0;
  }
};

TEST_F(ZeroCrossingsWindowTest, Do) {
  Do((*Input)[0], &(*Output)[0]);
  ASSERT_EQ(Size / 2 + 1, (*Output)[0]);
  int slowres = DoInternal(false, (*Input)[0], Size);
  ASSERT_EQ(Size / 2 + 1, slowres);
}

#define CLASS_NAME ZeroCrossingsWindowTest
#define ITER_COUNT 400000
#define NO_OUTPUT
#define BENCH_FUNC DoInternal
#include "tests/transforms/benchmark.inc"

class ZeroCrossingsRawTest : public TransformTest<ZeroCrossings16> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 1024;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      // Always liked exotic functions
      (*Input)[0][i] = sinf(i * M_PI / 2) * 1024;
    }
    (*Input)[0][3] = 1024;
  }
};

TEST_F(ZeroCrossingsRawTest, Do) {
  Do((*Input)[0], &(*Output)[0]);
  ASSERT_EQ(Size / 2, (*Output)[0]);
  int slowres = DoInternal(false, (*Input)[0], Size);
  ASSERT_EQ(Size / 2, slowres);
}

#undef CLASS_NAME
#define CLASS_NAME ZeroCrossingsRawTest
#undef INPUT_TYPE
#define INPUT_TYPE int16_t
#include "tests/transforms/benchmark.inc"
