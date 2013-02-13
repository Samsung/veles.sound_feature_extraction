/*! @file parameters.cc
 *  @brief Tests for src/parameters.h.
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
#include "src/primitives/convolute.h"
#include "src/primitives/memory.h"

#ifdef __AVX__

#include <immintrin.h>

void DebugPrint__m256(const char* name, __m256 vec) {
  float lanes[8];
  _mm256_store_ps(lanes, vec);
  printf("%s\t{ %f\t%f\t%f\t%f\t%f\t%f\t%f\t%f }\n", name,
         lanes[0], lanes[1], lanes[2], lanes[3],
         lanes[4], lanes[5], lanes[6], lanes[7]);
}

TEST(convolute, AVXComplexMultiplication) {
  float ar1[8] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
  float ar2[8] = { -1.5f, 1.0f, 3.5f, 3.0f, -5.5f, 6.5f, 2.0f, -9.0f };
  float res[8] = { 0.0f };
  __m256 Xvec = _mm256_load_ps(ar1);
  DebugPrint__m256("Xvec\t", Xvec);
  __m256 Hvec = _mm256_load_ps(ar2);
  DebugPrint__m256("Hvec\t", Hvec);
  __m256 Xim = _mm256_movehdup_ps(Xvec);
  DebugPrint__m256("Xim\t", Xim);
  __m256 Xre = _mm256_moveldup_ps(Xvec);
  DebugPrint__m256("Xre\t", Xre);
  __m256 HvecExch = _mm256_shuffle_ps(Hvec, Hvec, 0xB1);
  DebugPrint__m256("HvecExch", HvecExch);
  __m256 resHalf1 = _mm256_mul_ps(Xre, Hvec);
  DebugPrint__m256("resHalf1", resHalf1);
  __m256 resHalf2 = _mm256_mul_ps(Xim, HvecExch);
  DebugPrint__m256("resHalf2", resHalf2);
  __m256 resVec = _mm256_addsub_ps(resHalf1, resHalf2);
  DebugPrint__m256("resVec\t", resVec);
  _mm256_store_ps(res, resVec);

  float verif[8];
  for (int cci = 0; cci < 8; cci += 2) {
    float re1 = ar1[cci];
    float im1 = ar1[cci + 1];
    float re2 = ar2[cci];
    float im2 = ar2[cci + 1];
    verif[cci] = re1 * re2 - im1 * im2;
    verif[cci + 1] = re1 * im2 + re2 * im1;
  }
  printf("VERIFY\t\t{ %f\t%f\t%f\t%f\t%f\t%f\t%f\t%f }\n",
         verif[0], verif[1], verif[2], verif[3],
         verif[4], verif[5], verif[6], verif[7]);
  ASSERT_EQ(0, memcmp(res, verif, 8 * sizeof(float)));
}

#elif defined(__ARM_NEON__)

#include <arm_neon.h>

void DebugPrintFloat32x4_t(const char* name, float32x4_t vec) {
  float lanes[4];
  vst1q_f32(lanes, vec);
  printf("%s\t{ %f\t%f\t%f\t%f }\n", name,
         lanes[0], lanes[1], lanes[2], lanes[3]);
}

TEST(convolute, NeonComplexMultiplication) {
  float ar1[4] = { 1.0f, 2.0f, 3.0f, 4.0f };
  float ar2[4] = { -1.5f, 1.0f, 3.5f, 3.0f };
  float res[4] = { 0.0f };
  const float32x4_t negVec = { 1.0f, -1.0f, 1.0f, -1.0f };

  float32x4_t Xvec = vld1q_f32(ar1);
  DebugPrintFloat32x4_t("Xvec\t", Xvec);
  float32x4_t Hvec = vld1q_f32(ar2);
  DebugPrintFloat32x4_t("Hvec\t", Hvec);
  float32x4_t Xrev = vrev64q_f32(Xvec);
  DebugPrintFloat32x4_t("Xrev\t", Xrev);
  float32x4_t fwdMul = vmulq_f32(Xvec, Hvec);
  DebugPrintFloat32x4_t("fwdMul\t", fwdMul);
  float32x4_t resIm = vmulq_f32(Hvec, Xrev);
  DebugPrintFloat32x4_t("resIm\t", resIm);
  float32x4_t negMul = vmulq_f32(fwdMul, negVec);
  DebugPrintFloat32x4_t("negMul\t", negMul);
  float32x4x2_t resPair = vtrnq_f32(negMul, resIm);
  float32x4_t resVec = vaddq_f32(resPair.val[0], resPair.val[1]);
  DebugPrintFloat32x4_t("resVec\t", resVec);
  vst1q_f32(res, resVec);

  float verif[4];
  for (int cci = 0; cci < 4; cci += 2) {
    float re1 = ar1[cci];
    float im1 = ar1[cci + 1];
    float re2 = ar2[cci];
    float im2 = ar2[cci + 1];
    verif[cci] = re1 * re2 - im1 * im2;
    verif[cci + 1] = re1 * im2 + re2 * im1;
  }
  printf("VERIFY\t\t{ %f\t%f\t%f\t%f }\n",
         verif[0], verif[1], verif[2], verif[3]);
  ASSERT_EQ(0, memcmp(res, verif, 4 * sizeof(float)));
}

#endif

void convolute_reference(const float *__restrict x, size_t xLength,
                         const float *__restrict h, size_t hLength,
                         float *__restrict result) {
  for (int i = 0; i < (int)xLength; i++) {
    float sum = .0f;
    for (int j = 0; j < (int)hLength; j++) {
      sum += h[j] * x[i - j];
    }
    result[i] = sum;
  }
}

TEST(convolute, convolute) {
  float x[1024] = { 1.0f };
  float h[50];
  for (int i = 0; i < (int)(sizeof(h) / sizeof(float)); i++) {
    h[i] = i / (sizeof(h) / sizeof(float) - 1.0f);
  }
  float res[sizeof(x) / sizeof(float)];
  convolute(x, sizeof(x) / sizeof(float), h, sizeof(h) / sizeof(float), res);
  float verif[sizeof(x) / sizeof(float)];
  convolute_reference(x, sizeof(x) / sizeof(float),
                      h, sizeof(h) / sizeof(float), verif);

  float error = .0f;
  for (int i = 0; i < (int)(sizeof(x) / sizeof(float)); i++) {
    float delta = res[i] - verif[i];
    error += delta * delta;
  }
  printf("Error: %f\n", error);
}

#include "tests/google/src/gtest_main.cc"
