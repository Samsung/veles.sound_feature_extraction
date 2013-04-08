/*! @file wavelet.cc
 *  @brief Tests for src/primitives/wavelet.h.
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
#include <chrono>
#include "src/primitives/arithmetic-inl.h"
#define WAVELET_INTERNAL_USE
#include "src/primitives/daubechies.h"
#include "src/primitives/coiflets.h"
#include "src/primitives/symlets.h"
#include "src/primitives/memory.h"
#include "src/primitives/wavelet.h"

const int BENCHMARK_LENGTH = 100000;

TEST(Wavelet, wavelet_prepare_array) {
  float array[512];
  int length = sizeof(array) / sizeof(float);  // NOLINT(*)
  for (int i = 0; i < length; i++) {
    array[i] = i;
  }
  auto res = wavelet_prepare_array(8, array, length);

#ifdef __AVX__
  ASSERT_EQ(0, align_complement_f32(res));
  ASSERT_EQ(0, memcmp(array, res, length * sizeof(float)));  // NOLINT(*)
  int checkSize = (length - 8) * sizeof(float);  // NOLINT(*)
  ASSERT_EQ(0, memcmp(array + 2, res + length, checkSize));
  ASSERT_EQ(0, memcmp(array + 4, res + length * 2 - 8, checkSize));
  ASSERT_EQ(0, memcmp(array + 6, res + length * 3 - 16, checkSize));
#else
  ASSERT_EQ(0, memcmp(res, array, sizeof(array)));
#endif
  free(res);

  res = wavelet_prepare_array(4, array, length);

#ifdef __AVX__
  ASSERT_EQ(0, align_complement_f32(res));
  ASSERT_EQ(0, memcmp(array, res, length * sizeof(float)));  // NOLINT(*)
  ASSERT_EQ(0, memcmp(array + 2, res + length, checkSize));
#else
  ASSERT_EQ(0, memcmp(res, array, sizeof(array)));
#endif
  free(res);
}

TEST(Wavelet, wavelet_allocate_destination) {
  auto dest = wavelet_allocate_destination(8, 512);
#ifdef __AVX__
  ASSERT_EQ(0, align_complement_f32(dest));
#endif
  free(dest);
}

#define EPSILON 0.0005f

#define ASSERT_EQF(a, b) do { \
  ASSERT_GT(a + EPSILON, b); \
  ASSERT_LT(a - EPSILON, b); \
} while (0)

TEST(Wavelet, wavelet_apply_na) {
  float array[32], desthi[16], destlo[16];
  int length = sizeof(array) / sizeof(float);  // NOLINT(*)
  for (int i = 0; i < length; i++) {
    array[i] = i;
  }
  wavelet_apply_na(WAVELET_TYPE_DAUBECHIES, 8, array, length,
                   desthi, destlo);
  int index = 5;
  float vhi = .0f, vlo = .0f;
  for (int i = 0; i < 8; i++) {
    vlo += array[index * 2 + i] * kDaubechiesF[3][i];
    vhi += array[index * 2 + i] * kDaubechiesF[3][8 - i - 1] * (i & 1 ? -1 : 1);
  }
  ASSERT_EQF(vlo, destlo[index]);
  ASSERT_EQF(vhi, desthi[index]);
  vhi = vlo = .0f;
  for (int i = 0; i < 8; i++) {
    float value = .0f;
    switch (i) {
      case 0:
        value = array[30];
        break;
      case 1:
        value = array[31];
        break;
      case 2:
        value = array[0];
        break;
      case 3:
        value = array[1];
        break;
      case 4:
        value = array[2];
        break;
      case 5:
        value = array[3];
        break;
      case 6:
        value = array[4];
        break;
      case 7:
        value = array[5];
        break;
      default:
        break;
    }
    vlo += value * kDaubechiesF[3][i];
    vhi += value * kDaubechiesF[3][8 - i - 1] * (i & 1 ? -1 : 1);
  }
  ASSERT_EQF(vlo, destlo[15]);
  ASSERT_EQF(vhi, desthi[15]);

  wavelet_apply_na(WAVELET_TYPE_DAUBECHIES, 8, array, 8, desthi, destlo);
}

TEST(Wavelet, wavelet_apply) {
  float array[512];
  const int length = sizeof(array) / sizeof(float);  // NOLINT(*)
  for (int i = 0; i < length; i++) {
    array[i] = i;
  }
  auto prep = wavelet_prepare_array(8, array, length);
  auto desthi = wavelet_allocate_destination(8, length);
  auto destlo = wavelet_allocate_destination(8, length);

  std::vector<std::pair<WaveletType, std::vector<int>>> wavelets {  // NOLINT(*)
    { WAVELET_TYPE_DAUBECHIES, { 4, 6, 8, 12, 16 } },
    { WAVELET_TYPE_SYMLET, { 4, 6, 8, 12, 16 } },
    { WAVELET_TYPE_COIFLET, { 6, 12 } }
  };

  for (auto wp : wavelets) {
    for (int order : wp.second) {
      wavelet_apply(wp.first, order, prep, length, desthi, destlo);
      float validdesthi[length / 2], validdestlo[length / 2];
      wavelet_apply_na(wp.first, order, array, length,
                       validdesthi, validdestlo);
      for (int i = 0; i < length / 2; i++) {
        ASSERT_EQF(validdesthi[i], desthi[i]);
        ASSERT_EQF(validdestlo[i], destlo[i]);
      }
    }
  }

  free(desthi);
  free(destlo);
  free(prep);
}

#ifdef BENCHMARK
#ifdef SIMD
TEST(Wavelet, SIMDSpeedup) {
  float array[512];
  const int length = sizeof(array) / sizeof(float);  // NOLINT(*)
  for (int i = 0; i < length; i++) {
    array[i] = i;
  }

  std::vector<int> orders { 4, 6, 8, 12, 16 };  // NOLINT(*)

  for (int order : orders) {
    auto prep = wavelet_prepare_array(order, array, length);
    auto desthi = wavelet_allocate_destination(order, length);
    auto destlo = wavelet_allocate_destination(order, length);

    auto checkPointStart = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < BENCHMARK_LENGTH; i++) {
      wavelet_apply(WAVELET_TYPE_DAUBECHIES, order, prep, length, desthi, destlo);  // NOLINT(*)
    }

    auto checkPointFinish = std::chrono::high_resolution_clock::now();
    auto delta1 = checkPointFinish - checkPointStart;
    checkPointStart = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < BENCHMARK_LENGTH; i++) {
      wavelet_apply_na(WAVELET_TYPE_DAUBECHIES, order, array, length,
                       desthi, destlo);
    }

    checkPointFinish = std::chrono::high_resolution_clock::now();
    auto delta2 = checkPointFinish - checkPointStart;
    float ratio = (delta1.count() + .0f) / delta2.count();
    float speedup = (delta2.count() - delta1.count() + .0f) / delta2.count();
    printf("[order %i] SIMD version took %i%% of original time. "
        "Speedup is %i%%.\n", order,
        static_cast<int>(ratio * 100), static_cast<int>(speedup * 100));

    free(desthi);
    free(destlo);
    free(prep);
  }
}
#endif
#endif

#include "tests/google/src/gtest_main.cc"
