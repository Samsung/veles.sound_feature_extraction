/*! @file complex_to_real.cc
 *  @brief Tests for SoundFeatureExtraction::Transforms::Mean.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include <math.h>
#include "src/transforms/mean.h"
#include "tests/transforms/transform_test.h"

using SoundFeatureExtraction::Formats::RawFormatF;
using SoundFeatureExtraction::Formats::FixedArray;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Transforms::Mean;

class MeanTest : public TransformTest<Mean> {
 public:
  int Size;

  virtual void SetUp() {
    SetParameter("types", "arithmetic geometric");
    Size = 486;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i + 1;
    }
  }
};

#define EPSILON 0.15f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(MeanTest, Do) {
  SetUseSimd(false);
  Do((*Input)[0], &(*Output)[0]);

  float amean = 0.f;
  float gmean = 1.f, tmp = 1.f;
  for (int j = 0; j < Size; j++) {
    float val = (*Input)[0][j];
    amean += val;
    float multmp = tmp * val;
    if (multmp == std::numeric_limits<float>::infinity()) {
      gmean *= powf(tmp, 1.f / Size);
      tmp = val;
    } else {
      tmp = multmp;
    }
  }
  amean /= Size;
  gmean *= powf(tmp, 1.f / Size);
  ASSERT_EQF(amean, ((*Output)[0])
      [SoundFeatureExtraction::Transforms::MEAN_TYPE_ARITHMETIC]);
  ASSERT_EQF(gmean, ((*Output)[0])
      [SoundFeatureExtraction::Transforms::MEAN_TYPE_GEOMETRIC]);
  ASSERT_EQF(amean,
             Do(false, (*Input)[0], Size,
                SoundFeatureExtraction::Transforms::MEAN_TYPE_ARITHMETIC));
  ASSERT_EQF(gmean,
             Do(false, (*Input)[0], Size,
                SoundFeatureExtraction::Transforms::MEAN_TYPE_GEOMETRIC));
}

TEST_F(MeanTest, DoSimd) {
  SetUseSimd(true);
  Do((*Input)[0], &(*Output)[0]);

  float amean = 0.f;
  float gmean = 1.f, tmp = 1.f;
  for (int j = 0; j < Size; j++) {
    float val = (*Input)[0][j];
    amean += val;
    float multmp = tmp * val;
    if (multmp == std::numeric_limits<float>::infinity()) {
      gmean *= powf(tmp, 1.f / Size);
      tmp = val;
    } else {
      tmp = multmp;
    }
  }
  amean /= Size;
  gmean *= powf(tmp, 1.f / Size);
  ASSERT_EQF(amean, ((*Output)[0])
      [SoundFeatureExtraction::Transforms::MEAN_TYPE_ARITHMETIC]);
  ASSERT_EQF(gmean, ((*Output)[0])
      [SoundFeatureExtraction::Transforms::MEAN_TYPE_GEOMETRIC]);
  ASSERT_EQF(amean,
             Do(false, (*Input)[0], Size,
                SoundFeatureExtraction::Transforms::MEAN_TYPE_ARITHMETIC));
  ASSERT_EQF(gmean,
             Do(false, (*Input)[0], Size,
                SoundFeatureExtraction::Transforms::MEAN_TYPE_GEOMETRIC));
}

TEST_F(MeanTest, DoCase1) {
  float data[] {
       14.000000, 12.147677, 7.239147,  1.920708,  5.957093,  9.768391,  10.661250, 8.767072,  5.146831,  1.545739,
       2.542431,  4.095103,  4.268193,  3.487323,  2.419142,  1.679975,  1.614317,  1.932667,  2.242195,  2.394188,
       2.421191,  2.678555,  3.710246,  5.343172,  6.749712,  7.024582,  5.638990,  3.404799,  4.893416,  8.849478,
       11.533978, 11.468283, 8.327876,  3.028759,  3.826278,  8.811692,  11.308797, 10.646174, 7.323343,  2.797879,
       2.428811,  5.215474,  6.156565,  5.400490,  3.810879,  2.359146,  1.500396,  1.038677,  1.125559,  1.527126,
       1.637754,  1.585411,  1.999519,  2.711942,  3.145981,  3.151658,  3.015997,  3.240598,  4.112591,  5.435161,
       6.637612,  6.981294,  5.896019,  3.464489,  2.449490,  5.461041,  7.942804,  8.417841,  6.677486,  3.596703,
       2.589705,  4.832062,  6.014294,  5.606078,  4.626289,  4.552700,  5.004514,  4.624925,  3.121680,  1.513250,
       2.142658,  2.988013,  2.727604,  1.585353,  0.509548,  0.912408,  0.951881,  1.048167,  2.098451,  3.050678,
       3.265496,  2.714679,  2.034716,  2.226161,  2.983628,  3.635599,  4.015833,  3.936847,  3.112669,  1.524254,
       1.105299,  2.855775,  3.761478,  3.377796,  2.582373,  3.663632,  5.529967,  6.274077,  5.218201,  2.618501,
       1.319440,  4.050600,  5.615703,  5.424427,  3.769471,  1.476823,  0.505356,  1.479253,  1.335812,  0.505446,
       0.457828,  0.686575,  0.090077,  1.139614,  2.380332,  3.019486,  2.706917,  1.538594,  0.000000,  1.284005,
       1.865358,  1.694431,  1.121461,  0.597073,  0.554776,  1.616257,  3.098090,  4.259238,  4.394960,  3.152024,
       0.963656,  2.548137,  4.992091,  6.150478,  5.615703,  3.635787,  0.969018,  1.447651,  2.887414,  3.110871,
       2.414879,  1.442621,  0.789162,  0.516361,  0.830651,  1.468186,  1.716659,  1.220493,  0.256707,  1.473497,
       2.533228,  2.742794,  2.051204,  1.263791,  1.886841,  2.594487,  2.423380,  1.473448,  1.130886,  2.106285,
       2.655293,  2.586874,  2.647682,  3.307341,  3.804050,  3.440928,  2.142658,  0.595667,  1.673491,  2.725870,
       2.911726,  2.433653,  1.831784,  1.464580,  1.052485,  0.361370,  0.638862,  1.238473,  1.297600,  1.001795,
       1.338005,  2.103705,  2.449490,  2.144198,  1.560320,  1.557979,  1.920745,  1.877495,  1.616533,  2.071501,
       2.909563,  3.201581,  2.572886,  1.163561,  0.576751,  1.924525,  2.485965,  2.130837,  1.125559,  0.198161,
       0.928041,  1.240121,  1.267145,  1.458520,  1.674948,  1.475499,  0.767147,  0.634006,  1.587356,  2.181869,
       2.211601,  1.919891,  1.841200,  2.045926,  2.025416,  1.494594,  0.567949,  0.809057,  1.855393,  2.598861,
       2.877227,  2.632328,  1.914737,  1.100587,  1.402948,  2.319586,  2.835637,  2.710121,  2.109199,  1.587058,
       1.614317,  1.706014,  1.474589,  1.340186,  1.834860,  2.353730,  2.316422,  1.580782,  0.418109,  1.134244,
       2.182069,  2.637870,  2.458133,  1.933196,  1.648814,  1.842487,  2.000000
  };
  Size = sizeof(data) / sizeof(data[0]);
  SetParameter("types", "geometric");
  SetUpTransform(1, Size, 18000);
  memcpy((*Input)[0], data, sizeof(data));
  Do((*Input)[0], &(*Output)[0]);
  ASSERT_NE(std::numeric_limits<float>::infinity(), (*Output)[0][1]);
}

#define EXTRA_PARAM SoundFeatureExtraction::Transforms::MEAN_TYPE_ARITHMETIC
#define CLASS_NAME MeanTest
#define ITER_COUNT 500000
#define NO_OUTPUT
#define BENCH_NAME BenchmarkArithmetic
#include "tests/transforms/benchmark.inc"

#undef EXTRA_PARAM
#define EXTRA_PARAM SoundFeatureExtraction::Transforms::MEAN_TYPE_GEOMETRIC
#undef BENCH_NAME
#define BENCH_NAME BenchmarkGeometric
#include "tests/transforms/benchmark.inc"
