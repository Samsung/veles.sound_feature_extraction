/*! @file spectralenergy.cc
 *  @brief Tests for sound_feature_extraction::transforms::SpectralEnergy.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include <cmath>
#include "src/transforms/spectral_energy.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::SpectralEnergy;

class SpectralEnergyTest : public TransformTest<SpectralEnergy> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 378;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i / 40.0f;
    }
  }
};

#define EPSILON 0.0001f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(SpectralEnergyTest, Do) {
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size / 2; i++) {
    float m = (*Output)[0][i];
    float re = i * 2;
    float im = i * 2 + 1;
    ASSERT_EQF((re * re + im * im) / 1600.0f, m);
  }
}

#define CLASS_NAME SpectralEnergyTest
#define ITER_COUNT 500000
#include "tests/transforms/benchmark.inc"
