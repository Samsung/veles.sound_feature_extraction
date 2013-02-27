/*! @file arithmetic.cc
 *  @brief Tests for src/primitives/arithmetic-inl.h.
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
#include "src/primitives/arithmetic-inl.h"

TEST(Arithmetic, int16_multiply) {
  int16_t a[16], b[16] __attribute__ ((aligned (64)));
  int32_t res[16] __attribute__ ((aligned (64)));
  for (int i = 0; i < 16; i++) {
    a[i] = i;
    b[i] = 16 - i;
    res[i] = 0;
  }
  int16_multiply(a, b, res);
//  for (int i = 0; i < 8; i++) {
//    printf("%i\t", res[i]);
//  }
  printf("\n");
  ASSERT_EQ(0, res[0]);
  ASSERT_EQ(1 * 15, res[1]);
  ASSERT_EQ(2 * 14, res[2]);
  ASSERT_EQ(3 * 13, res[3]);
#ifdef __AVX__
  ASSERT_EQ(4 * 12, res[4]);
  ASSERT_EQ(5 * 11, res[5]);
  ASSERT_EQ(6 * 10, res[6]);
  ASSERT_EQ(7 *  9, res[7]);
#ifdef __AVX2__
  ASSERT_EQ(8 *  8, res[8]);
  ASSERT_EQ(9 *  7, res[9]);
  ASSERT_EQ(10*  6, res[10]);
  ASSERT_EQ(11*  5, res[11]);
  ASSERT_EQ(12*  4, res[12]);
  ASSERT_EQ(13*  3, res[13]);
  ASSERT_EQ(14*  2, res[14]);
  ASSERT_EQ(15*  1, res[15]);
#endif
#endif
}

/*
void DebugPrint__m256(const char* name, __m256 vec) {
  float lanes[8];
  _mm256_store_ps(lanes, vec);
  printf("%s\t{ %f\t%f\t%f\t%f\t%f\t%f\t%f\t%f }\n", name,
         lanes[0], lanes[1], lanes[2], lanes[3],
         lanes[4], lanes[5], lanes[6], lanes[7]);
}

void DebugPrintFloat32x4_t(const char* name, float32x4_t vec) {
  float lanes[4];
  vst1q_f32(lanes, vec);
  printf("%s\t{ %f\t%f\t%f\t%f }\n", name,
         lanes[0], lanes[1], lanes[2], lanes[3]);
}
*/

TEST(Arithmetic, complex_multiply) {
  float ar1[8] __attribute__ ((aligned (64))) = {
      1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f
  };
  float ar2[8] __attribute__ ((aligned (64))) = {
      -1.5f, 1.0f, 3.5f, 3.0f, -5.5f, 6.5f, 2.0f, -9.0f
  };
  float res[8] __attribute__ ((aligned (64)));
  /*
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
  */
  /*
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
  */

  complex_multiply(ar1, ar2, res);

  float verif[8];
  for (int cci = 0; cci < 8; cci += 2) {
    complex_multiply_na(&ar1[cci], &ar2[cci], &verif[cci]);
  }
  // printf("VERIFY\t\t{ %f\t%f\t%f\t%f\t%f\t%f\t%f\t%f }\n",
  //        verif[0], verif[1], verif[2], verif[3],
  //        verif[4], verif[5], verif[6], verif[7]);
  // printf("VERIFY\t\t{ %f\t%f\t%f\t%f }\n",
  //        verif[0], verif[1], verif[2], verif[3]);
  ASSERT_EQ(0, memcmp(res, verif,
#ifdef __AVX__
                      8
#elif defined(__NEON__)
                      4
#else
                      2
#endif
                      * sizeof(float)));
}

TEST(Arithmetic, real_multiply) {
  float ar1[8] __attribute__ ((aligned (64))) = {
      1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f
  };
  float ar2[8] __attribute__ ((aligned (64))) = {
      -1.5f, 1.0f, 3.5f, 3.0f, -5.5f, 6.5f, 2.0f, -9.0f
  };
  float res[8] __attribute__ ((aligned (64)));

  real_multiply(ar1, ar2, res);

  float verif[8];
  for (int i = 0; i < 8; i++) {
    verif[i] = ar1[i] * ar2[i];
  }
  ASSERT_EQ(0, memcmp(res, verif,
  #ifdef __AVX__
                        8
  #elif defined(__NEON__)
                        4
  #else
                        1
  #endif
                        * sizeof(float)));
}

TEST(Arithmetic, real_multiply_scalar) {
  float ar[19] __attribute__ ((aligned (32))) = {
      1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f,
      11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f
  };
  float res[19] __attribute__ ((aligned (32)));
  real_multiply_scalar(&ar[1], 18, 2.0f, &res[1]);
  float verif[18];
  real_multiply_scalar_na(&ar[1], 18, 2.0f, verif);
  ASSERT_EQ(0, memcmp(&res[1], verif, 18 * sizeof(float)));
#ifdef __AVX__
  EXPECT_DEATH({
    real_multiply_scalar(&ar[1], 18, 2.0f, res);
  }, ".*Assertion.*");
#endif
}

TEST(Arithmetic, int16_to_int32) {
  int16_t ar[30] __attribute__ ((aligned (32))) = {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
      11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
      21, 22, 23, 24, 25, 26, 27, 28, 29, 30
  };
  int32_t res[30] __attribute__ ((aligned (32)));
  int16_to_int32(&ar[1], 29, &res[1]);
  int32_t verif[29];
  int16_to_int32_na(&ar[1], 29, verif);
  ASSERT_EQ(0, memcmp(&res[1], verif, 29 * sizeof(int32_t)));
#ifdef __AVX__
    EXPECT_DEATH({
      int16_to_int32(&ar[1], 29, res);
    }, ".*Assertion.*");
#endif
}

TEST(Arithmetic, int32_to_int16) {
  int32_t ar[30] __attribute__ ((aligned (32))) = {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
      11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
      21, 22, 23, 24, 25, 26, 27, 28, 29, 30
  };
  int16_t res[30] __attribute__ ((aligned (32)));
  int32_to_int16(&ar[1], 29, &res[1]);
  int16_t verif[29];
  int32_to_int16_na(&ar[1], 29, verif);
  ASSERT_EQ(0, memcmp(&res[1], verif, 29 * sizeof(int16_t)));
#ifdef __AVX__
    EXPECT_DEATH({
      int32_to_int16(&ar[1], 29, res);
    }, ".*Assertion.*");
#endif
}

TEST(Arithmetic, int32_to_float) {
  int32_t ar[31] __attribute__ ((aligned (32))) = {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
      11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
      21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
  };
  float res[30] __attribute__ ((aligned (32)));
  int32_to_float(&ar[1], 29, &res[1]);
  float verif[29];
  int32_to_float_na(&ar[1], 29, verif);
  ASSERT_EQ(0, memcmp(&res[1], verif, 29 * sizeof(float)));
#ifdef __AVX__
    EXPECT_DEATH({
      int32_to_float(&ar[1], 29, res);
    }, ".*Assertion.*");
#endif
}

TEST(Arithmetic, float_to_int32) {
  float ar[31] __attribute__ ((aligned (32))) = {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
      11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
      21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
  };
  int32_t res[30] __attribute__ ((aligned (32)));
  float_to_int32(&ar[1], 29, &res[1]);
  int32_t verif[29];
  float_to_int32_na(&ar[1], 29, verif);
  ASSERT_EQ(0, memcmp(&res[1], verif, 29 * sizeof(float)));
#ifdef __AVX__
    EXPECT_DEATH({
      float_to_int32(&ar[1], 29, res);
    }, ".*Assertion.*");
#endif
}

TEST(Arithmetic, int16_to_float) {
  int16_t ar[31] __attribute__ ((aligned (32))) = {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
      11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
      21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
  };
  float res[30] __attribute__ ((aligned (32)));
  int16_to_float(&ar[1], 29, &res[1]);
  float verif[29];
  int16_to_float_na(&ar[1], 29, verif);
  ASSERT_EQ(0, memcmp(&res[1], verif, 29 * sizeof(float)));
#ifdef __AVX__
    EXPECT_DEATH({
      int16_to_float(&ar[1], 29, res);
    }, ".*Assertion.*");
#endif
}

TEST(Arithmetic, float_to_int16) {
  float ar[31] __attribute__ ((aligned (32))) = {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
      11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
      21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
  };
  int16_t res[30] __attribute__ ((aligned (32)));
  float_to_int16(&ar[1], 29, &res[1]);
  int16_t verif[29];
  float_to_int16_na(&ar[1], 29, verif);
  ASSERT_EQ(0, memcmp(&res[1], verif, 29 * sizeof(int16_t)));
#ifdef __AVX__
    EXPECT_DEATH({
      float_to_int16(&ar[1], 29, res);
    }, ".*Assertion.*");
#endif
}

#include "tests/google/src/gtest_main.cc"
