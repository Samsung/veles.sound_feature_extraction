/*! @file convolute.cc
 *  @brief Tests for src/primitives/convolute.cc.
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
#include <math.h>
#include "src/primitives/convolute.h"
#include "src/primitives/memory.h"
#include "src/primitives/arithmetic-inl.h"

void convolute_reference(const float *__restrict x, size_t xLength,
                         const float *__restrict h, size_t hLength,
                         float *__restrict result) {
  for (int n = 0; n < static_cast<int>(xLength); n++) {
    float sum = 0.f;
    for (int m = 0; m < static_cast<int>(hLength) && m <= n; m++) {
      sum += h[m] * x[n - m];
    }
    result[n] = sum;
  }
}

void DebugPrintConvolution(const char* name, const float* vec) {
  printf("%s\t", name);
  for (int i = 0; i < 10; i++) {
    printf("%f  ", vec[i]);
  }
  printf("\n");
}

TEST(convolute, convolute_fft) {
  const int xlen = 1020;
  const int hlen = 50;

  float x[xlen];
  for (int i = 0; i < xlen; i++) {
    x[i] = sinf(i) * 100;
  }
  float h[hlen];
  for (int i = 0; i < hlen; i++) {
    h[i] = i / (hlen- 1.0f);
  }

  float verif[xlen];
  convolute_reference(x, xlen, h, hlen, verif);
  DebugPrintConvolution("REFERENCE", verif);

  float res[xlen];
  convolute_fft(x, xlen, h, hlen, res);
  DebugPrintConvolution("FFT\t", res);

  int firstDifferenceIndex = -1;
  for (int i = 0; i < xlen; i++) {
    float delta = res[i] - verif[i];
    if (delta * delta > 1E-6 && firstDifferenceIndex == -1) {
      firstDifferenceIndex = i;
    }
  }
  ASSERT_EQ(-1, firstDifferenceIndex);
}

TEST(convolute, convolute_overlap_save) {
  const int xlen = 1021;
  const int hlen = 50;

  float x[xlen];
  for (int i = 0; i < xlen; i++) {
    x[i] = sinf(i) * 100;
  }
  float h[hlen];
  for (int i = 0; i < hlen; i++) {
    h[i] = i / (hlen- 1.0f);
  }

  float verif[xlen];
  convolute_reference(x, xlen, h, hlen, verif);
  DebugPrintConvolution("REFERENCE", verif);

  float res[xlen];
  convolute_overlap_save(x, xlen, h, hlen, res);
  DebugPrintConvolution("OVERLAP-SAVE", res);

  int firstDifferenceIndex = -1;
  for (int i = 0; i < xlen; i++) {
    float delta = res[i] - verif[i];
    if (delta * delta > 1E-6 && firstDifferenceIndex == -1) {
      firstDifferenceIndex = i;
    }
  }
  ASSERT_EQ(-1, firstDifferenceIndex);
}

TEST(convolute, convolute_simd) {
  const int xlen = 1024;
  const int hlen = 50;

  float x[xlen];
  for (int i = 0; i < xlen; i++) {
    x[i] = sinf(i) * 100;
  }
  float h[hlen];
  for (int i = 0; i < hlen; i++) {
    h[i] = i / (hlen - 1.0f);
  }

  float verif[xlen];
  convolute_reference(x, xlen, h, hlen, verif);

  float res[xlen];
  convolute_simd(true, x, xlen, h, hlen, res);

  int firstDifferenceIndex = -1;
  for (int i = 0; i < xlen; i++) {
    float delta = res[i] - verif[i];
    if (delta * delta > 1E-6 && firstDifferenceIndex == -1) {
      firstDifferenceIndex = i;
    }
  }
  ASSERT_EQ(-1, firstDifferenceIndex);

  convolute_simd(false, x, xlen, h, hlen, res);

  for (int i = 0; i < xlen; i++) {
    float delta = res[i] - verif[i];
    if (delta * delta > 1E-6 && firstDifferenceIndex == -1) {
      firstDifferenceIndex = i;
    }
  }
  ASSERT_EQ(-1, firstDifferenceIndex);
}

float BenchmarkH1[50] = { 1.f };
float BenchmarkH2[500] = { 1.f };
float BenchmarkResult[10000];

#define TEST_NAME convolute_simd_50
#define ITER_COUNT 50000
#define BENCH_FUNC convolute_simd
#define NO_OUTPUT
#define EXTRA_PARAM BenchmarkH1, sizeof(BenchmarkH1) / sizeof(BenchmarkH1[0]), \
  BenchmarkResult
#include "tests/transforms/benchmark.inc"

#undef EXTRA_PARAM
#undef ITER_COUNT
#undef TEST_NAME
#define TEST_NAME convolute_simd_500
#define ITER_COUNT 10000
#define EXTRA_PARAM BenchmarkH2, sizeof(BenchmarkH2) / sizeof(BenchmarkH2[0]), \
  BenchmarkResult
#include "tests/transforms/benchmark.inc"
/*
#undef LENGTH
#define LENGTH 500
#define CUSTOM_FUNC_BASELINE(x, xLength) convolute_simd(\
    true, x, xLength, BenchmarkH1, sizeof(BenchmarkH1) / sizeof(BenchmarkH1[0]), \
    BenchmarkResult)
#define CUSTOM_FUNC_PEAK(x, xLength) convolute_fft(\
    x, xLength, BenchmarkH1, sizeof(BenchmarkH1) / sizeof(BenchmarkH1[0]), \
    BenchmarkResult)
#undef ITER_COUNT
#undef TEST_NAME
#define TEST_NAME convolute_simd_vs_convolute_fft_500_50
#define ITER_COUNT 15000
#include "tests/transforms/benchmark.inc"

#undef ITER_COUNT
#define ITER_COUNT 1000
#undef LENGTH
#define LENGTH 10000
#undef TEST_NAME
#define TEST_NAME convolute_simd_vs_convolute_fft_5000_50
#undef CUSTOM_FUNC_BASELINE
#define CUSTOM_FUNC_BASELINE(x, xLength) convolute_overlap_save(\
    x, xLength, BenchmarkH1, 50, \
    BenchmarkResult)
#undef CUSTOM_FUNC_PEAK
#define CUSTOM_FUNC_PEAK(x, xLength) convolute_fft(\
    x, xLength, BenchmarkH1, 50, \
    BenchmarkResult)
#include "tests/transforms/benchmark.inc"*/

#include "tests/google/src/gtest_main.cc"
