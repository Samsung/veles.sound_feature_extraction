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
  for (int i = 0; i < 40; i++) {
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
  auto handle = convolute_fft_prepare(xlen, hlen);
  convolute_fft(handle, x, h, res);
  convolute_fft_finalize(handle);
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
  auto handle = convolute_overlap_save_prepare(xlen, hlen);
  convolute_overlap_save(handle, x, h, res);
  convolute_overlap_save_finalize(handle);
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

TEST(convolute, convolute_ones) {
  const int xlen = 1024;
  const int hlen = 1024;

  float x[xlen];
    for (int i = 0; i < xlen; i++) {
      x[i] = sinf(i) * 100;
    }
    float h[hlen];
    int step = 8;
    int pulses = 3;
    int ct = 0;
    for (int i = 0; i < hlen; i++) {
      if (i % step == 0 && ct < pulses) {
        h[i] = 1;
        ct++;
      } else
        h[i] = 0;
    }

    float verif[xlen];
    convolute_reference(x, xlen, h, hlen, verif);
    DebugPrintConvolution("REFERENCE", verif);

    float res[xlen];
    convolute_ones(x, xlen, step, pulses, res);
    DebugPrintConvolution("CONVOLUTE-ONES", res);

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

float BenchmarkH[512] = { 1.f };
float BenchmarkResult[10000];

#define TEST_NAME convolute_simd_50
#define ITER_COUNT 50000
#define BENCH_FUNC convolute_simd
#define NO_OUTPUT
#define EXTRA_PARAM BenchmarkH, sizeof(BenchmarkH) / sizeof(BenchmarkH[0]), \
  BenchmarkResult
#include "tests/transforms/benchmark.inc"

#undef EXTRA_PARAM
#undef ITER_COUNT
#undef TEST_NAME
#define TEST_NAME convolute_simd_500
#define ITER_COUNT 10000
#define EXTRA_PARAM BenchmarkH, sizeof(BenchmarkH) / sizeof(BenchmarkH[0]), \
  BenchmarkResult
#include "tests/transforms/benchmark.inc"

ConvoluteFFTHandle fftHandle;

#undef OPT_STRING
#define OPT_STRING "FFT"
#undef LENGTH
#define LENGTH 512
#define CUSTOM_FUNC_BASELINE(x, xLength) convolute_simd(\
    true, x, xLength, BenchmarkH, 512, BenchmarkResult)
#define CUSTOM_FUNC_PEAK(x, xLength) convolute_fft(\
    fftHandle, x, BenchmarkH, BenchmarkResult)
#undef ITER_COUNT
#undef TEST_NAME
#define TEST_NAME convolute_simd_vs_convolute_fft_512_512
#define ITER_COUNT 25000
#define CUSTOM_CODE_PRE { fftHandle = convolute_fft_prepare(512, 512); }
#define CUSTOM_CODE_POST { convolute_fft_finalize(fftHandle); }
#include "tests/transforms/benchmark.inc"

#undef LENGTH
#define LENGTH 256
#undef CUSTOM_FUNC_BASELINE
#define CUSTOM_FUNC_BASELINE(x, xLength) convolute_simd(\
    true, x, xLength, BenchmarkH, 256, BenchmarkResult)
#undef CUSTOM_FUNC_PEAK
#define CUSTOM_FUNC_PEAK(x, xLength) convolute_fft(\
    fftHandle, x, BenchmarkH, BenchmarkResult)
#undef ITER_COUNT
#undef TEST_NAME
#define TEST_NAME convolute_simd_vs_convolute_fft_256_256
#define ITER_COUNT 60000
#undef CUSTOM_CODE_PRE
#define CUSTOM_CODE_PRE { fftHandle = convolute_fft_prepare(256, 256); }
#include "tests/transforms/benchmark.inc"

#undef LENGTH
#define LENGTH 128
#undef CUSTOM_FUNC_BASELINE
#define CUSTOM_FUNC_BASELINE(x, xLength) convolute_simd(\
    true, x, xLength, BenchmarkH, 128, BenchmarkResult)
#undef CUSTOM_FUNC_PEAK
#define CUSTOM_FUNC_PEAK(x, xLength) convolute_fft(\
    fftHandle, x, BenchmarkH, BenchmarkResult)
#undef ITER_COUNT
#undef TEST_NAME
#define TEST_NAME convolute_simd_vs_convolute_fft_128_128
#define ITER_COUNT 80000
#undef CUSTOM_CODE_PRE
#define CUSTOM_CODE_PRE { fftHandle = convolute_fft_prepare(128, 128); }
#include "tests/transforms/benchmark.inc"

#ifdef __AVX__
#undef LENGTH
#define LENGTH 350
#undef CUSTOM_FUNC_BASELINE
#define CUSTOM_FUNC_BASELINE(x, xLength) convolute_simd(\
    true, x, xLength, BenchmarkH, 350, BenchmarkResult)
#undef CUSTOM_FUNC_PEAK
#define CUSTOM_FUNC_PEAK(x, xLength) convolute_fft(\
    fftHandle, x, BenchmarkH, BenchmarkResult)
#undef ITER_COUNT
#undef TEST_NAME
#define TEST_NAME convolute_simd_vs_convolute_fft_350_350
#define ITER_COUNT 40000
#undef CUSTOM_CODE_PRE
#define CUSTOM_CODE_PRE { fftHandle = convolute_fft_prepare(350, 350); }
#include "tests/transforms/benchmark.inc"
#endif

ConvoluteOverlapSaveHandle osHandle;

#undef OPT_STRING
#define OPT_STRING "Overlap-Save"
#undef LENGTH
#define LENGTH 1000
#undef CUSTOM_FUNC_BASELINE
#define CUSTOM_FUNC_BASELINE(x, xLength) convolute_fft(\
    fftHandle, x, BenchmarkH, BenchmarkResult)
#undef CUSTOM_FUNC_PEAK
#define CUSTOM_FUNC_PEAK(x, xLength) convolute_overlap_save(\
    osHandle, x, BenchmarkH, BenchmarkResult)
#undef ITER_COUNT
#undef TEST_NAME
#define TEST_NAME convolute_fft_vs_convolute_overlap_save_1000_50
#define ITER_COUNT 30000
#undef CUSTOM_CODE_PRE
#define CUSTOM_CODE_PRE { osHandle = convolute_overlap_save_prepare(1000, 50); \
                          fftHandle = convolute_fft_prepare(1000, 50); }
#undef CUSTOM_CODE_POST
#define CUSTOM_CODE_POST { convolute_overlap_save_finalize(osHandle); \
                           convolute_fft_finalize(fftHandle); }
#include "tests/transforms/benchmark.inc"

#undef LENGTH
#define LENGTH 2000
#undef CUSTOM_FUNC_BASELINE
#define CUSTOM_FUNC_BASELINE(x, xLength) convolute_fft(\
    fftHandle, x, BenchmarkH, BenchmarkResult)
#undef CUSTOM_FUNC_PEAK
#define CUSTOM_FUNC_PEAK(x, xLength) convolute_overlap_save(\
    osHandle, x, BenchmarkH, BenchmarkResult)
#undef ITER_COUNT
#undef TEST_NAME
#define TEST_NAME convolute_fft_vs_convolute_overlap_save_2000_950
#define ITER_COUNT 10000
#undef CUSTOM_CODE_PRE
#define CUSTOM_CODE_PRE { osHandle = convolute_overlap_save_prepare(2000, 950); \
                          fftHandle = convolute_fft_prepare(2000, 950); }
#undef CUSTOM_CODE_POST
#define CUSTOM_CODE_POST { convolute_overlap_save_finalize(osHandle); \
                           convolute_fft_finalize(fftHandle); }
#include "tests/transforms/benchmark.inc"

#undef LENGTH
#define LENGTH 1000
#undef CUSTOM_FUNC_BASELINE
#define CUSTOM_FUNC_BASELINE(x, xLength) convolute_simd(\
    true, x, xLength, BenchmarkH, 50, BenchmarkResult)
#undef CUSTOM_FUNC_PEAK
#define CUSTOM_FUNC_PEAK(x, xLength) convolute_overlap_save(\
    osHandle, x, BenchmarkH, BenchmarkResult)
#undef ITER_COUNT
#undef TEST_NAME
#define TEST_NAME convolute_simd_vs_convolute_overlap_save_1000_50
#define ITER_COUNT 20000
#undef CUSTOM_CODE_PRE
#define CUSTOM_CODE_PRE { osHandle = convolute_overlap_save_prepare(1000, 50); }
#undef CUSTOM_CODE_POST
#define CUSTOM_CODE_POST { convolute_overlap_save_finalize(osHandle); }
#include "tests/transforms/benchmark.inc"

#undef LENGTH
#define LENGTH 200
#undef CUSTOM_FUNC_BASELINE
#define CUSTOM_FUNC_BASELINE(x, xLength) convolute_simd(\
    true, x, xLength, BenchmarkH, 50, BenchmarkResult)
#undef CUSTOM_FUNC_PEAK
#define CUSTOM_FUNC_PEAK(x, xLength) convolute_overlap_save(\
    osHandle, x, BenchmarkH, BenchmarkResult)
#undef ITER_COUNT
#undef TEST_NAME
#define TEST_NAME convolute_simd_vs_convolute_overlap_save_200_50
#define ITER_COUNT 80000
#undef CUSTOM_CODE_PRE
#define CUSTOM_CODE_PRE { osHandle = convolute_overlap_save_prepare(200, 50); }
#undef CUSTOM_CODE_POST
#define CUSTOM_CODE_POST { convolute_overlap_save_finalize(osHandle); }
#include "tests/transforms/benchmark.inc"

#include "tests/google/src/gtest_main.cc"
