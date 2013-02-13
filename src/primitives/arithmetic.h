/*! @file arithmetic.h
 *  @brief Basic arithmetic functions with SIMD acceleration.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef ARITHMETIC_H_
#define ARITHMETIC_H_

#define complex_multiply_na(a, b, res) do { \
  float re1 = (a)[0]; \
  float im1 = (a)[1]; \
  float re2 = (b)[0]; \
  float im2 = (b)[1]; \
  (result)[0] = re1 * re2 - im1 * im2; \
  (result)[1] = re1 * im2 + re2 * im1; \
} while (0)

#ifdef __AVX__

#define complex_multiply(a, b, res) do { \
  __m256 Xvec = _mm256_load_ps(a); \
  __m256 Hvec = _mm256_load_ps(b); \
  __m256 Xim = _mm256_movehdup_ps(Xvec); \
  __m256 Xre = _mm256_moveldup_ps(Xvec); \
  __m256 HvecExch = _mm256_shuffle_ps(Hvec, Hvec, 0xB1); \
  __m256 resHalf1 = _mm256_mul_ps(Xre, Hvec); \
  __m256 resHalf2 = _mm256_mul_ps(Xim, HvecExch); \
  __m256 resVec = _mm256_addsub_ps(resHalf1, resHalf2); \
  _mm256_store_ps(res, resVec); \
} while (0)

#elif defined(__ARM_NEON__)

#define complex_multiply(a, b, res) do { \
  const float32x4_t negVec = { 1.0f, -1.0f, 1.0f, -1.0f }; \
  float32x4_t Xvec = vld1q_f32(a); \
  float32x4_t Hvec = vld1q_f32(b); \
  float32x4_t Xrev = vrev64q_f32(Xvec); \
  float32x4_t fwdMul = vmulq_f32(Xvec, Hvec); \
  float32x4_t resIm = vmulq_f32(Hvec, Xrev); \
  float32x4_t negMul = vmulq_f32(fwdMul, negVec); \
  float32x4x2_t resPair = vtrnq_f32(negMul, resIm); \
  float32x4_t resVec = vaddq_f32(resPair.val[0], resPair.val[1]); \
  vst1q_f32(res, resVec); \
} while (0)

#else

#define complex_multiply complex_multiply_na

#endif

#endif  // INCLUDE_ARITHMETIC_H_
