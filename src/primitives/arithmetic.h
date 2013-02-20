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

#ifdef __cplusplus
#define __STDC_LIMIT_MACROS
#endif
#ifdef __AVX__
#include <assert.h>
#endif
#include <stdint.h>
#include "src/primitives/memory.h"

INLINE NOTNULL((1, 3)) void int16_to_float_na(const int16_t *data,
                                              size_t length, float *res) {
  for (size_t i = 0; i < length; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL((1, 3)) void float_to_int16_na(const float *data,
                                              size_t length, int16_t *res) {
  for (size_t i = 0; i < length; i++) {
    res[i] = (int16_t)data[i];
  }
}

INLINE NOTNULL((1, 3)) void int32_to_float_na(const int32_t *data,
                                              size_t length, float *res) {
  for (size_t i = 0; i < length; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL((1, 3)) void float_to_int32_na(const float *data,
                                              size_t length, int32_t *res) {
  for (size_t i = 0; i < length; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL((1, 3)) void int32_to_int16_na(const int32_t *data,
                                              size_t length, int16_t *res) {
  for (size_t i = 0; i < length; i++) {
    res[i] = (int16_t)data[i];
  }
}

INLINE NOTNULL((1, 3)) void int16_to_int32_na(const int16_t *data,
                                              size_t length, int32_t *res) {
  for (size_t i = 0; i < length; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL((1, 2, 3)) void complex_multiply_na(
    const float *a, const float *b, float *res) {
  float re1 = (a)[0];
  float im1 = a[1];
  float re2 = b[0];
  float im2 = b[1];
  res[0] = re1 * re2 - im1 * im2;
  res[1] = re1 * im2 + re2 * im1;
}

INLINE NOTNULL((3)) void real_multiply_scalar_na(
    float value, size_t arrayLength, float *array) {
  for (size_t i = 0; i < arrayLength; i++) {
    array[i] *= value;
  }
}

#ifdef __AVX__

#include <immintrin.h>

#ifdef __AVX2__

/// @brief Multiplies the contents of two vectors, saving the result to the
/// third vector, using AVX2 SIMD (int16_t doubling version).
/// @details res[i] = a[i] * b[i], i = 0..15.
/// @param a First vector.
/// @param b Second vector.
/// @param res Result vector.
/// @pre a, b and res must be aligned to 32 bytes.
INLINE NOTNULL((1, 2, 3)) void int16_multiply(
    const int16_t *a, const int16_t *b, int32_t *res) {
  __m256i aVec = _mm256_load_si256((const __m256i*)a);
  __m256i bVec = _mm256_load_si256((const __m256i*)b);
  __m256i resVecHi = _mm256_mulhi_epi16(aVec, bVec);
  __m256i resVecLo = _mm256_mullo_epi16(aVec, bVec);
  _mm256_maskstore_epi32(
      res, _mm256_set_epi16(-1, 0,-1, 0,-1, 0,-1, 0,-1, 0,-1, 0,-1, 0,-1, 0),
      resVecHi);
  _mm256_maskstore_epi32(
      res, _mm256_set_epi16( 0,-1, 0,-1, 0,-1, 0,-1, 0,-1, 0,-1, 0,-1, 0,-1),
      resVecLo);
}

INLINE NOTNULL((1, 3)) void int16_to_float(const int16_t *data,
                                           size_t length, float *res) {
  int startIndex = align_complement_i16(data);
  assert(startIndex * 2 == align_complement_f32(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (float)data[i];
  }

  for (size_t i = startIndex; i < length - 15; i += 16) {
    __m256i intVec = _mm256_load_si256((const __m256i*)(data + i));
    __m256i intlo = _mm256_unpacklo_epi16(intVec, _mm256_set1_epi16(0));
    __m256i inthi = _mm256_unpackhi_epi16(intVec, _mm256_set1_epi16(0));
    __m256 flo = _mm256_cvtepi32_ps(intlo);
    __m256 fhi = _mm256_cvtepi32_ps(inthi);
    _mm256_store_ps(res + i, flo);
    _mm256_store_ps(res + i + 8, fhi);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 4) << 4);
      i < length; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL((1, 3)) void float_to_int16(const float *data,
                                           size_t length, int16_t *res) {
  int startIndex = align_complement_f32(data);
  assert(startIndex == 2 * align_complement_i16(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (int16_t)data[i];
  }

  for (size_t i = startIndex; i < length - 15; i += 16) {
    __m256 fVecHi = _mm256_load_ps(data + i);
    __m256 fVecLo = _mm256_load_ps(data + i + 8);
    __m256i intVecHi = _mm256_cvtps_epi32(fVecHi);
    __m256i intVecLo = _mm256_cvtps_epi32(fVecLo);
    __m256i int16Vec = _mm256_packs_epi32(intVecHi, intVecLo);
    _mm256_store_si256((__m256i *)(res + i), int16Vec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 4) << 4);
       i < length; i++) {
    res[i] = (int16_t)data[i];
  }
}

INLINE NOTNULL((1, 3)) void int32_to_float(const int32_t *data,
                                           size_t length, float *res) {
  int startIndex = align_complement_i32(data);
  assert(startIndex == align_complement_f32(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (float)data[i];
  }

  for (size_t i = startIndex; i < length - 7; i += 8) {
    __m256i intVec = _mm256_load_si256((const __m256i*)(data + i));
    __m256 fVec = _mm256_cvtepi32_ps(intVec);
    _mm256_store_ps(res + i, fVec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 3) << 3);
       i < length; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL((1, 3)) void float_to_int32(const float *data,
                                           size_t length, int32_t *res) {
  int startIndex = align_complement_f32(data);
  assert(startIndex == align_complement_i32(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (int16_t)data[i];
  }

  for (size_t i = startIndex; i < length - 7; i += 8) {
    __m256 fVec = _mm256_load_ps(data + i);
    __m256i intVec = _mm256_cvtps_epi32(fVec);
    _mm256_store_si256((__m256i *)(res + i), intVec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 3) << 3);
       i < length; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL((1, 3)) void int16_to_int32(const int16_t *data,
                                           size_t length, int32_t *res) {
  int startIndex = align_complement_i16(data);
  assert(startIndex * 2 == align_complement_i32(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (float)data[i];
  }

  for (size_t i = startIndex; i < length - 15; i += 16) {
    __m256i intVec = _mm256_load_si256((const __m256i*)(data + i));
    __m256i intlo = _mm256_unpacklo_epi16(intVec, _mm256_set1_epi16(0));
    __m256i inthi = _mm256_unpackhi_epi16(intVec, _mm256_set1_epi16(0));
    _mm256_store_si256((__m256i *)(res + i), inthi);
    _mm256_store_si256((__m256i *)(res + i + 8), intlo);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 4) << 4);
      i < length; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL((1, 3)) void int32_to_int16(const int32_t *data,
                                           size_t length, int16_t *res) {
  int startIndex = align_complement_i32(data);
  assert(startIndex == 2 * align_complement_i16(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (int16_t)data[i];
  }

  for (size_t i = startIndex; i < length - 15; i += 16) {
    __m256i intVecHi = _mm256_load_si256((const __m256i*)(data + i));
    __m256i intVecLo = _mm256_load_si256((const __m256i*)(data + i + 4));
    __m256i int16Vec = _mm256_packs_epi32(intVecHi, intVecLo);
    _mm256_store_si256((__m256i *)(res + i), int16Vec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 4) << 4);
        i < length; i++) {
      res[i] = (int16_t)data[i];
  }
}

#else

INLINE NOTNULL((1, 3)) void int16_to_float(const int16_t *data,
                                           size_t length, float *res) {
  int startIndex = align_complement_i16(data);
  assert(startIndex * 2 == align_complement_f32(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (float)data[i];
  }

  for (size_t i = startIndex; i < length - 7; i += 8) {
    __m128i intVec = _mm_load_si128((const __m128i*)(data + i));
    __m128i intlo = _mm_unpacklo_epi16(intVec, _mm_set1_epi16(0));
    __m128i inthi = _mm_unpackhi_epi16(intVec, _mm_set1_epi16(0));
    __m128 flo = _mm_cvtepi32_ps(intlo);
    __m128 fhi = _mm_cvtepi32_ps(inthi);
    _mm_store_ps(res + i, flo);
    _mm_store_ps(res + i + 4, fhi);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 3) << 3);
      i < length; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL((1, 3)) void float_to_int16(const float *data,
                                           size_t length, int16_t *res) {
  int startIndex = align_complement_f32(data);
  assert(startIndex == 2 * align_complement_i16(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (int16_t)data[i];
  }

  for (size_t i = startIndex; i < length - 7; i += 8) {
    __m128 fVecHi = _mm_load_ps(data + i);
    __m128 fVecLo = _mm_load_ps(data + i + 4);
    __m128i intVecHi = _mm_cvtps_epi32(fVecHi);
    __m128i intVecLo = _mm_cvtps_epi32(fVecLo);
    __m128i int16Vec = _mm_packs_epi32(intVecHi, intVecLo);
    _mm_store_si128((__m128i *)(res + i), int16Vec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 3) << 3);
       i < length; i++) {
    res[i] = (int16_t)data[i];
  }
}

INLINE NOTNULL((1, 3)) void int32_to_float(const int32_t *data,
                                           size_t length, float *res) {
  int startIndex = align_complement_i32(data);
  assert(startIndex == align_complement_f32(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (float)data[i];
  }

  for (size_t i = startIndex; i < length - 3; i += 4) {
    __m128i intVec = _mm_load_si128((const __m128i*)(data + i));
    __m128 fVec = _mm_cvtepi32_ps(intVec);
    _mm_store_ps(res + i, fVec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 2) << 2);
       i < length; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL((1, 3)) void float_to_int32(const float *data,
                                           size_t length, int32_t *res) {
  int startIndex = align_complement_f32(data);
  assert(startIndex == align_complement_i32(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (int16_t)data[i];
  }

  for (size_t i = startIndex; i < length - 3; i += 4) {
    __m128 fVec = _mm_load_ps(data + i);
    __m128i intVec = _mm_cvtps_epi32(fVec);
    _mm_store_si128((__m128i *)(res + i), intVec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 2) << 2);
       i < length; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL((1, 3)) void int16_to_int32(const int16_t *data,
                                           size_t length, int32_t *res) {
  int startIndex = align_complement_i16(data);
  assert(startIndex * 2 == align_complement_i32(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (float)data[i];
  }

  for (size_t i = startIndex; i < length - 7; i += 8) {
    __m128i intVec = _mm_load_si128((const __m128i*)(data + i));
    __m128i intlo = _mm_unpacklo_epi16(intVec, _mm_set1_epi16(0));
    __m128i inthi = _mm_unpackhi_epi16(intVec, _mm_set1_epi16(0));
    _mm_store_si128((__m128i *)(res + i), inthi);
    _mm_store_si128((__m128i *)(res + i + 4), intlo);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 3) << 3);
      i < length; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL((1, 3)) void int32_to_int16(const int32_t *data,
                                           size_t length, int16_t *res) {
  int startIndex = align_complement_i32(data);
  assert(startIndex == 2 * align_complement_i16(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (int16_t)data[i];
  }

  for (size_t i = startIndex; i < length - 7; i += 8) {
    __m128i intVecHi = _mm_load_si128((const __m128i*)(data + i));
    __m128i intVecLo = _mm_load_si128((const __m128i*)(data + i + 4));
    __m128i int16Vec = _mm_packs_epi32(intVecHi, intVecLo);
    _mm_store_si128((__m128i *)(res + i), int16Vec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 3) << 3);
        i < length; i++) {
      res[i] = (int16_t)data[i];
  }
}

/// @brief Multiplies the contents of two vectors, saving the result to the
/// third vector, using SSSE3 SIMD (int16_t doubling version).
/// @details res[i] = a[i] * b[i], i = 0..7.
/// @param a First vector.
/// @param b Second vector.
/// @param res Result vector.
/// @pre a, b and res must be aligned to 16 bytes.
INLINE NOTNULL((1, 2, 3)) void int16_multiply(
    const int16_t *a, const int16_t *b, int32_t *res) {
  __m128i aVec = _mm_load_si128((const __m128i*)a);
  __m128i bVec = _mm_load_si128((const __m128i*)b);
  __m128i resVecHi = _mm_mulhi_epi16(aVec, bVec);
  __m128i resVecLo = _mm_mullo_epi16(aVec, bVec);
  _mm_maskmoveu_si128(resVecHi, _mm_set_epi16(-1, 0,-1, 0,-1, 0,-1, 0),
                      (char*)res);
  _mm_maskmoveu_si128(resVecLo, _mm_set_epi16( 0,-1, 0,-1, 0,-1, 0,-1),
                      (char*)res);
}

#endif

/// @brief Multiplies the contents of two vectors, saving the result to the
/// third vector, using AVX SIMD (float version).
/// @details res[i] = a[i] * b[i], i = 0..7.
/// @param a First vector.
/// @param b Second vector.
/// @param res Result vector.
/// @pre a, b and res must be aligned to 32 bytes.
INLINE NOTNULL((1, 2, 3)) void real_multiply(
    const float *a, const float *b, float *res) {
  __m256 aVec = _mm256_load_ps(a);
  __m256 bVec = _mm256_load_ps(b);
  __m256 resVec = _mm256_mul_ps(aVec, bVec);
  _mm256_store_ps(res, resVec);
}

/// @brief Performs complex multiplication of the contents of two complex
/// vectors, saving the result to the third vector, using AVX SIMD.
/// @details res[i] = a[i] * b[i] - a[i + 1] * b[i + 1], i = 0, 2, 4, 6;
/// res[i + 1] = a[i] * b[i + 1] + a[i + 1] * b[i], i = 1, 3, 5, 7.
/// @param a First vector.
/// @param b Second vector.
/// @param res Result vector.
/// @pre a, b and res must be aligned to 32 bytes.
INLINE NOTNULL((1, 2, 3)) void complex_multiply(
    const float *a, const float *b, float *res) {
  __m256 Xvec = _mm256_load_ps(a);
  __m256 Hvec = _mm256_load_ps(b);
  __m256 Xim = _mm256_movehdup_ps(Xvec);
  __m256 Xre = _mm256_moveldup_ps(Xvec);
  __m256 HvecExch = _mm256_shuffle_ps(Hvec, Hvec, 0xB1);
  __m256 resHalf1 = _mm256_mul_ps(Xre, Hvec);
  __m256 resHalf2 = _mm256_mul_ps(Xim, HvecExch);
  __m256 resVec = _mm256_addsub_ps(resHalf1, resHalf2);
  _mm256_store_ps(res, resVec);
}

INLINE NOTNULL((3)) void real_multiply_scalar(float value, size_t arrayLength,
                                              float *array) {
  int startIndex = align_complement_f32(array);

  for (int i = 0; i < startIndex; i++) {
    array[i] *= value;
  }

  const __m256 mulVec = _mm256_set_ps( value, value, value, value,
                                       value, value, value, value );
  for (size_t i = (int)startIndex; i < arrayLength - 7; i += 8) {
    __m256 vec = _mm256_load_ps(array + i);
    vec = _mm256_mul_ps(vec, mulVec);
    _mm256_store_ps(array + i, vec);
  }

  for (size_t i = startIndex + (((arrayLength - startIndex) >> 3) << 3);
      i < arrayLength; i++) {
    array[i] *= value;
  }
}

#elif defined(__ARM_NEON__)

#include <arm_neon.h>

INLINE NOTNULL((1, 3)) void int16_to_float(const int16_t *data,
                                           size_t length, float *res) {
  for (size_t i = 0; i < length - 3; i += 4) {
    int16x4_t intVec = vld1_s16(data + i);
    int32x4_t extIntVec = vmovl_s16(intVec);
    float32x4_t floatVec = vcvtq_f32_s32(extIntVec);
    vst1q_f32(res + i, floatVec);
  }
  for (int i = ((length >> 2) << 2); i < length; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL((1, 3)) void float_to_int16(const float *data,
                                           size_t length, int16_t *res) {
  for (size_t i = 0; i < length - 3; i += 4) {
    float32x4_t floatVec = vld1q_f32(data + i);
    int32x4_t extIntVec = vcvtq_s32_f32(floatVec);
    int16x4_t intVec = vqmovn_s32(extIntVec);
    vst1_s16(res + i, intVec);
  }
  for (int i = ((length >> 2) << 2); i < length; i++) {
    res[i] = (int16_t)data[i];
  }
}

INLINE NOTNULL((1, 3)) void int32_to_float(const int32_t *data,
                                           size_t length, float *res) {
  for (size_t i = 0; i < length - 3; i += 4) {
    int32x4_t intVec = vld1q_s32(data + i);
    float32x4_t floatVec = vcvtq_f32_s32(intVec);
    vst1q_f32(res + i, floatVec);
  }
  for (int i = ((length >> 2) << 2); i < length; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL((1, 3)) void float_to_int32(const float *data,
                                           size_t length, int32_t *res) {
  for (size_t i = 0; i < length - 3; i += 4) {
    float32x4_t floatVec = vld1q_f32(data + i);
    int32x4_t intVec = vcvtq_s32_f32(floatVec);
    vst1q_s32(res + i, intVec);
  }
  for (int i = ((length >> 2) << 2); i < length; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL((1, 3)) void int16_to_int32(const int16_t *data,
                                           size_t length, int32_t *res) {
  for (size_t i = 0; i < length - 3; i += 4) {
    int16x4_t intVec = vld1_s16(data + i);
    int32x4_t extIntVec = vmovl_s16(intVec);
    vst1q_s32(res + i, extIntVec);
  }
  for (int i = ((length >> 2) << 2); i < length; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL((1, 3)) void int32_to_int16(const int32_t *data,
                                           size_t length, int16_t *res) {
  for (size_t i = 0; i < length - 3; i += 4) {
    int32x4_t extIntVec = vld1q_s32(data + i);
    int16x4_t intVec = vqmovn_s32(extIntVec);
    vst1_s16(res + i, intVec);
  }
  for (int i = ((length >> 2) << 2); i < length; i++) {
    res[i] = (int16_t)data[i];
  }
}

/// @brief Multiplies the contents of two vectors, saving the result to the
/// third vector, using NEON SIMD (int16_t doubling version).
/// @details res[i] = a[i] * b[i], i = 0..3.
/// @param a First vector.
/// @param b Second vector.
/// @param res Result vector.
INLINE NOTNULL((1, 2, 3)) void int16_multiply(
    const int16_t *a, const int16_t *b, int32_t *res) {
  int16x4_t aVec = vld1_s16(a);
  int16x4_t bVec = vld1_s16(b);
  int32x4_t resVec = vmull_s16(aVec, bVec);
  vst1q_s32(res, resVec);
}

/// @brief Multiplies the contents of two vectors, saving the result to the
/// third vector, using NEON SIMD (float version).
/// @details res[i] = a[i] * b[i], i = 0..7.
/// @param a First vector.
/// @param b Second vector.
/// @param res Result vector.
INLINE NOTNULL((1, 2, 3)) void real_multiply(
    const float *a, const float *b, float *res) {
  float32x4_t aVec = vld1q_f32(a);
  float32x4_t bVec = vld1q_f32(b);
  float32x4_t resVec = vmulq_f32(aVec, bVec);
  vst1q_f32(res, resVec);
}

/// @brief Performs complex multiplication of the contents of two complex
/// vectors, saving the result to the third vector, using NEON SIMD.
/// @details res[i] = a[i] * b[i] - a[i + 1] * b[i + 1], i = 0, 2, 4, 6;
/// res[i + 1] = a[i] * b[i + 1] + a[i + 1] * b[i], i = 1, 3, 5, 7.
/// @param a First vector.
/// @param b Second vector.
/// @param res Result vector.
INLINE NOTNULL((1, 2, 3)) void complex_multiply(
    const float *a, const float *b, float *res) {
  const float32x4_t negVec = { 1.0f, -1.0f, 1.0f, -1.0f };
  float32x4_t Xvec = vld1q_f32(a);
  float32x4_t Hvec = vld1q_f32(b);
  float32x4_t Xrev = vrev64q_f32(Xvec);
  float32x4_t fwdMul = vmulq_f32(Xvec, Hvec);
  float32x4_t resIm = vmulq_f32(Hvec, Xrev);
  float32x4_t negMul = vmulq_f32(fwdMul, negVec);
  float32x4x2_t resPair = vtrnq_f32(negMul, resIm);
  float32x4_t resVec = vaddq_f32(resPair.val[0], resPair.val[1]);
  vst1q_f32(res, resVec);
}

INLINE NOTNULL((3)) void real_multiply_scalar(float value, size_t arrayLength,
                                              float *array) {
  for (int i = 0; i < arrayLength - 3; i += 4) {
    float32x4_t vec = vld1q_f32(array + i);
    vec = vmulq_n_f32(vec, value);
    vst1q_f32(array + i, vec);
  }
  for (int i = ((arrayLength >> 2) << 2); i < arrayLength; i++) {
    array[i] *= value;
  }
}

#else

#define int16_to_float int16_to_float_na
#define int32_to_float int32_to_float_na
#define float_to_int16 float_to_int16_na
#define float_to_int32 float_to_int32_na
#define int32_to_int16 int32_to_int16_na
#define int16_to_int32 int16_to_int32_na
#define complex_multiply complex_multiply_na
#define real_multiply_scalar real_multiply_scalar_na

#endif

#endif  // INCLUDE_ARITHMETIC_H_
