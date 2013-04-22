/*! @file arithmetic-inl.h
 *  @brief Inline arithmetic functions with SIMD acceleration.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_PRIMITIVES_ARITHMETIC_INL_H_
#define SRC_PRIMITIVES_ARITHMETIC_INL_H_

#include "src/config.h"

#ifdef __cplusplus
#define __STDC_LIMIT_MACROS
#include "src/primitives/avx_extra.h"
#endif

#ifdef __AVX__
#include <assert.h>
#endif
#include <stdint.h>
#include "src/primitives/memory.h"

INLINE NOTNULL(1, 3) void int16_to_float_na(const int16_t *data,
                                            size_t length, float *res) {
  for (size_t i = 0; i < length; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL(1, 3) void float_to_int16_na(const float *data,
                                            size_t length, int16_t *res) {
  for (size_t i = 0; i < length; i++) {
    // Simple truncation here (fast).
    // If changed to roundf(), replace *cvttps* -> *cvtps*.
    res[i] = (int16_t)data[i];
  }
}

INLINE NOTNULL(1, 3) void int32_to_float_na(const int32_t *data,
                                            size_t length, float *res) {
  for (size_t i = 0; i < length; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL(1, 3) void float_to_int32_na(const float *data,
                                            size_t length, int32_t *res) {
  for (size_t i = 0; i < length; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL(1, 3) void int32_to_int16_na(const int32_t *data,
                                            size_t length, int16_t *res) {
  for (size_t i = 0; i < length; i++) {
    res[i] = (int16_t)data[i];
  }
}

INLINE NOTNULL(1, 3) void int16_to_int32_na(const int16_t *data,
                                            size_t length, int32_t *res) {
  for (size_t i = 0; i < length; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL(1, 2, 3) void real_multiply_na(
    const float *a, const float *b, float *res) {
  *res = *a * *b;
}

INLINE NOTNULL(1, 2, 3) void complex_multiply_na(
    const float *a, const float *b, float *res) {
  float re1 = (a)[0];
  float im1 = a[1];
  float re2 = b[0];
  float im2 = b[1];
  res[0] = re1 * re2 - im1 * im2;
  res[1] = re1 * im2 + re2 * im1;
}

INLINE NOTNULL(1, 4) void real_multiply_scalar_na(const float *array,
                                                  size_t length,
                                                  float value, float *res) {
  for (size_t i = 0; i < length; i++) {
    res[i] = array[i] * value;
  }
}

INLINE NOTNULL(1) float sum_na(const float *input, size_t length) {
  float res = 0.f;
  for (int j = 0; j < (int)length; j++) {
    res += input[j];
  }
  return res;
}

#ifdef __AVX__

#include <immintrin.h>  // NOLINT(build/include_order)

#define SIMD
#define FLOAT_STEP 8
#define FLOAT_STEP_LOG2 3

#ifdef __AVX2__

#define INT16MUL_STEP 16
#define INT16MUL_STEP_LOG2 4

/// @brief Multiplies the contents of two vectors, saving the result to the
/// third vector, using AVX2 SIMD (int16_t doubling version).
/// @details res[i] = a[i] * b[i], i = 0..15.
/// @param a First vector.
/// @param b Second vector.
/// @param res Result vector.
/// @pre a, b and res must be aligned to 32 bytes.
INLINE NOTNULL(1, 2, 3) void int16_multiply(
    const int16_t *a, const int16_t *b, int32_t *res) {
  __m256i aVec = _mm256_load_si256((const __m256i*)a);
  __m256i bVec = _mm256_load_si256((const __m256i*)b);
  __m256i resVecHiP = _mm256_mulhi_epi16(aVec, bVec);
  __m256i resVecLoP = _mm256_mullo_epi16(aVec, bVec);
  __m256i resVecHi = _mm256_unpackhi_epi16(resVecLoP, resVecHiP);
  __m256i resVecLo = _mm256_unpacklo_epi16(resVecLoP, resVecHiP);
  _mm256_store_si256((__m256i *)res, resVecLo);
  _mm256_store_si256((__m256i *)(res + 8), resVecHi);
}

/// @brief Converts an array of short integers to floating point numbers,
/// using AVX2 SIMD.
/// @param data The array of short integers.
/// @param length The length of the array (in int16_t-s, not in bytes).
/// @param res The floating point number array to write the results to.
/// @note align_complement_i16(data) % 8 must be equal to
/// align_complement_f32(res) % 8.
/// @note res must have at least the same length as data.
INLINE NOTNULL(1, 3) void int16_to_float(const int16_t *data,
                                         size_t length, float *res) {
  int startIndex = align_complement_i16(data);
  assert(startIndex % 8 == align_complement_f32(res) % 8);
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

INLINE NOTNULL(1, 3) void float_to_int16(const float *data,
                                         size_t length, int16_t *res) {
  int startIndex = align_complement_f32(data);
  assert(startIndex % 16 == align_complement_i16(res) % 16);
  for (int i = 0; i < startIndex; i++) {
    res[i] = (int16_t)data[i];
  }

  for (size_t i = startIndex; i < length - 15; i += 16) {
    __m256 fVecHi = _mm256_load_ps(data + i);
    __m256 fVecLo = _mm256_load_ps(data + i + 8);
    __m256i intVecHi = _mm256_cvttps_epi32(fVecHi);
    __m256i intVecLo = _mm256_cvttps_epi32(fVecLo);
    __m256i int16Vec = _mm256_packs_epi32(intVecHi, intVecLo);
    _mm256_store_si256((__m256i *)(res + i), int16Vec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 4) << 4);
       i < length; i++) {
    res[i] = (int16_t)data[i];
  }
}

INLINE NOTNULL(1, 3) void int32_to_float(const int32_t *data,
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

INLINE NOTNULL(1, 3) void float_to_int32(const float *data,
                                         size_t length, int32_t *res) {
  int startIndex = align_complement_f32(data);
  assert(startIndex == align_complement_i32(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (int16_t)data[i];
  }

  for (size_t i = startIndex; i < length - 7; i += 8) {
    __m256 fVec = _mm256_load_ps(data + i);
    __m256i intVec = _mm256_cvttps_epi32(fVec);
    _mm256_store_si256((__m256i *)(res + i), intVec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 3) << 3);
       i < length; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL(1, 3) void int16_to_int32(const int16_t *data,
                                         size_t length, int32_t *res) {
  int startIndex = align_complement_i16(data);
  assert(startIndex % 8 == align_complement_i32(res) % 8);
  for (int i = 0; i < startIndex; i++) {
    res[i] = (float)data[i];
  }

  for (size_t i = startIndex; i < length - 15; i += 16) {
    __m256i intVec = _mm256_load_si256((const __m256i*)(data + i));
    __m256i intlo = _mm256_unpacklo_epi16(intVec, _mm256_set1_epi16(0));
    __m256i inthi = _mm256_unpackhi_epi16(intVec, _mm256_set1_epi16(0));
    _mm256_store_si256((__m256i *)(res + i), intlo);
    _mm256_store_si256((__m256i *)(res + i + 8), inthi);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 4) << 4);
      i < length; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL(1, 3) void int32_to_int16(const int32_t *data,
                                         size_t length, int16_t *res) {
  int startIndex = align_complement_i32(data);
  assert(startIndex % 16 == align_complement_i16(res) % 16);
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

#define INT16MUL_STEP 8
#define INT16MUL_STEP_LOG2 3

/// @brief Multiplies the contents of two vectors, saving the result to the
/// third vector, using SSSE3 SIMD (int16_t doubling version).
/// @details res[i] = a[i] * b[i], i = 0..7.
/// @param a First vector.
/// @param b Second vector.
/// @param res Result vector.
/// @pre a, b and res must be aligned to 16 bytes.
INLINE NOTNULL(1, 2, 3) void int16_multiply(
    const int16_t *a, const int16_t *b, int32_t *res) {
  __m128i aVec = _mm_load_si128((const __m128i*)a);
  __m128i bVec = _mm_load_si128((const __m128i*)b);
  __m128i resVecHiP = _mm_mulhi_epi16(aVec, bVec);
  __m128i resVecLoP = _mm_mullo_epi16(aVec, bVec);
  __m128i resVecHi = _mm_unpackhi_epi16(resVecLoP, resVecHiP);
  __m128i resVecLo = _mm_unpacklo_epi16(resVecLoP, resVecHiP);
  _mm_store_si128((__m128i *)res, resVecLo);
  _mm_store_si128((__m128i *)(res + 4), resVecHi);
}

/// @brief Converts an array of short integers to floating point numbers,
/// using SSE2 SIMD.
/// @param data The array of short integers.
/// @param length The length of the array (in int16_t-s, not in bytes).
/// @param res The floating point number array to write the results to.
/// @note align_complement_i16(data) % 4 must be equal to
/// align_complement_f32(res) % 4.
/// @note res must have at least the same length as data.
INLINE NOTNULL(1, 3) void int16_to_float(const int16_t *data,
                                         size_t length, float *res) {
  int startIndex = align_complement_i16(data);
  assert(startIndex % 4 == align_complement_f32(res) % 4);
  for (int i = 0; i < startIndex; i++) {
    res[i] = (float)data[i];
  }

  for (size_t i = startIndex; i < length - 7; i += 8) {
    __m128i intVec = _mm_load_si128((const __m128i*)(data + i));
    // Be careful with the sign bit as it should remain on the leftmost place
    __m128i intlo = _mm_unpacklo_epi16(_mm_set1_epi16(0), intVec);
    __m128i inthi = _mm_unpackhi_epi16(_mm_set1_epi16(0), intVec);
   intlo = _mm_sra_epi32(intlo, _mm_set1_epi32(16));
   inthi = _mm_sra_epi32(inthi, _mm_set1_epi32(16));
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

INLINE NOTNULL(1, 3) void float_to_int16(const float *data,
                                         size_t length, int16_t *res) {
  int startIndex = align_complement_f32(data);
  assert(startIndex % 8 == align_complement_i16(res) % 8);
  for (int i = 0; i < startIndex; i++) {
    res[i] = (int16_t)data[i];
  }

  for (size_t i = startIndex; i < length - 7; i += 8) {
    __m128 fVecHi = _mm_load_ps(data + i);
    __m128 fVecLo = _mm_load_ps(data + i + 4);
    __m128i intVecHi = _mm_cvttps_epi32(fVecHi);
    __m128i intVecLo = _mm_cvttps_epi32(fVecLo);
    __m128i int16Vec = _mm_packs_epi32(intVecHi, intVecLo);
    _mm_store_si128((__m128i *)(res + i), int16Vec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 3) << 3);
       i < length; i++) {
    res[i] = (int16_t)data[i];
  }
}

INLINE NOTNULL(1, 3) void int32_to_float(const int32_t *data,
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

INLINE NOTNULL(1, 3) void float_to_int32(const float *data,
                                         size_t length, int32_t *res) {
  int startIndex = align_complement_f32(data);
  assert(startIndex == align_complement_i32(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = (int16_t)data[i];
  }

  for (size_t i = startIndex; i < length - 3; i += 4) {
    __m128 fVec = _mm_load_ps(data + i);
    __m128i intVec = _mm_cvttps_epi32(fVec);
    _mm_store_si128((__m128i *)(res + i), intVec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 2) << 2);
       i < length; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL(1, 3) void int16_to_int32(const int16_t *data,
                                         size_t length, int32_t *res) {
  int startIndex = align_complement_i16(data);
  assert(startIndex % 4 == align_complement_i32(res) % 4);
  for (int i = 0; i < startIndex; i++) {
    res[i] = (float)data[i];
  }

  for (size_t i = startIndex; i < length - 7; i += 8) {
    __m128i intVec = _mm_load_si128((const __m128i*)(data + i));
    // Be careful with the sign bit as it should remain on the leftmost place
    __m128i intlo = _mm_unpacklo_epi16(_mm_set1_epi16(0), intVec);
    __m128i inthi = _mm_unpackhi_epi16(_mm_set1_epi16(0), intVec);
   intlo = _mm_sra_epi32(intlo, _mm_set1_epi32(16));
   inthi = _mm_sra_epi32(inthi, _mm_set1_epi32(16));
    _mm_store_si128((__m128i *)(res + i), intlo);
    _mm_store_si128((__m128i *)(res + i + 4), inthi);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 3) << 3);
      i < length; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL(1, 3) void int32_to_int16(const int32_t *data,
                                         size_t length, int16_t *res) {
  int startIndex = align_complement_i32(data);
  assert(startIndex % 8 == align_complement_i16(res) % 8);
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

#endif

/// @brief Multiplies the contents of two vectors, saving the result to the
/// third vector, using AVX SIMD (float version).
/// @details res[i] = a[i] * b[i], i = 0..7.
/// @param a First vector.
/// @param b Second vector.
/// @param res Result vector.
/// @pre a, b and res must be aligned to 32 bytes.
INLINE NOTNULL(1, 2, 3) void real_multiply(
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
INLINE NOTNULL(1, 2, 3) void complex_multiply(
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

/// @brief Multiplies each floating point number in the specified array
/// by the specified value, using AVX SIMD.
/// @details This functions does the same thing as real_multiply_scalar_na, but
/// likely much faster.
/// @param array The array of floating point numbers.
/// @param length The length of the array (in float-s, not in bytes).
/// @param value The value to multiply each number in array.
/// @param res The array to write the results to.
/// @note array and res must have the same alignment.
/// @note res must have at least the same length as array.
INLINE NOTNULL(1, 4) void real_multiply_scalar(const float *array,
                                               size_t length,
                                               float value, float *res) {
  int startIndex = align_complement_f32(array);
  assert(startIndex == align_complement_f32(res));
  for (int i = 0; i < startIndex; i++) {
    res[i] = array[i] * value;
  }

  const __m256 mulVec = _mm256_set_ps(value, value, value, value,
                                      value, value, value, value);
  for (size_t i = (int)startIndex; i < length - 7; i += 8) {
    __m256 vec = _mm256_load_ps(array + i);
    vec = _mm256_mul_ps(vec, mulVec);
    _mm256_store_ps(res + i, vec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 3) << 3);
      i < length; i++) {
    res[i] = array[i] * value;
  }
}

INLINE NOTNULL(1) float sum(const float *input, size_t length) {
  assert(align_complement_f32(input) == 0);
  int ilength = (int)length;
  __m256 accum = _mm256_setzero_ps();
  for (int j = 0; j < ilength - 15; j += 16) {
    __m256 vec1 = _mm256_load_ps(input + j);
    __m256 vec2 = _mm256_load_ps(input + j + 8);
    accum = _mm256_add_ps(accum, vec1);
    accum = _mm256_add_ps(accum, vec2);
  }
  accum = _mm256_hadd_ps(accum, accum);
  accum = _mm256_hadd_ps(accum, accum);
#ifdef __cplusplus
  float res = ElementAt(accum, 0) + ElementAt(accum, 4);
#else
  float res = accum[0] + accum[4];
#endif
  for (int j = ((ilength >> 4) << 4); j < ilength; j++) {
    res += input[j];
  }
  return res;
}

#elif defined(__ARM_NEON__)

#include <arm_neon.h>  // NOLINT(build/include_order)

#define SIMD
#define FLOAT_STEP 4
#define FLOAT_STEP_LOG2 2
#define INT16MUL_STEP 4
#define INT16MUL_STEP_LOG2 2

/// @brief Multiplies the contents of two vectors, saving the result to the
/// third vector, using NEON SIMD (int16_t doubling version).
/// @details res[i] = a[i] * b[i], i = 0..3.
/// @param a First vector.
/// @param b Second vector.
/// @param res Result vector.
INLINE NOTNULL(1, 2, 3) void int16_multiply(
    const int16_t *a, const int16_t *b, int32_t *res) {
  int16x4_t aVec = vld1_s16(a);
  int16x4_t bVec = vld1_s16(b);
  int32x4_t resVec = vmull_s16(aVec, bVec);
  vst1q_s32(res, resVec);
}

INLINE NOTNULL(1, 3) void int16_to_float(const int16_t *data,
                                         size_t length, float *res) {
  int ilength = (int)length;
  for (int i = 0; i < ilength - 3; i += 4) {
    int16x4_t intVec = vld1_s16(data + i);
    int32x4_t extIntVec = vmovl_s16(intVec);
    float32x4_t floatVec = vcvtq_f32_s32(extIntVec);
    vst1q_f32(res + i, floatVec);
  }
  for (int i = ((ilength >> 2) << 2); i < (int)length; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL(1, 3) void float_to_int16(const float *data,
                                         size_t length, int16_t *res) {
  int ilength = (int)length;
  for (int i = 0; i < ilength - 3; i += 4) {
    float32x4_t floatVec = vld1q_f32(data + i);
    int32x4_t extIntVec = vcvtq_s32_f32(floatVec);
    int16x4_t intVec = vqmovn_s32(extIntVec);
    vst1_s16(res + i, intVec);
  }
  for (int i = ((ilength >> 2) << 2); i < ilength; i++) {
    res[i] = (int16_t)data[i];
  }
}

INLINE NOTNULL(1, 3) void int32_to_float(const int32_t *data,
                                         size_t length, float *res) {
  int ilength = (int)length;
  for (int i = 0; i < ilength - 3; i += 4) {
    int32x4_t intVec = vld1q_s32(data + i);
    float32x4_t floatVec = vcvtq_f32_s32(intVec);
    vst1q_f32(res + i, floatVec);
  }
  for (int i = ((ilength >> 2) << 2); i < ilength; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL(1, 3) void float_to_int32(const float *data,
                                         size_t length, int32_t *res) {
  int ilength = (int)length;
  for (int i = 0; i < ilength - 3; i += 4) {
    float32x4_t floatVec = vld1q_f32(data + i);
    int32x4_t intVec = vcvtq_s32_f32(floatVec);
    vst1q_s32(res + i, intVec);
  }
  for (int i = ((ilength >> 2) << 2); i < ilength; i++) {
    res[i] = (int32_t)data[i];
  }
}

INLINE NOTNULL(1, 3) void int16_to_int32(const int16_t *data,
                                         size_t length, int32_t *res) {
  int ilength = (int)length;
  for (int i = 0; i < ilength - 3; i += 4) {
    int16x4_t intVec = vld1_s16(data + i);
    int32x4_t extIntVec = vmovl_s16(intVec);
    vst1q_s32(res + i, extIntVec);
  }
  for (int i = ((ilength >> 2) << 2); i < ilength; i++) {
    res[i] = (float)data[i];
  }
}

INLINE NOTNULL(1, 3) void int32_to_int16(const int32_t *data,
                                           size_t length, int16_t *res) {
  int ilength = (int)length;
  for (int i = 0; i < ilength - 3; i += 4) {
    int32x4_t extIntVec = vld1q_s32(data + i);
    int16x4_t intVec = vqmovn_s32(extIntVec);
    vst1_s16(res + i, intVec);
  }
  for (int i = ((ilength >> 2) << 2); i < ilength; i++) {
    res[i] = (int16_t)data[i];
  }
}

/// @brief Multiplies the contents of two vectors, saving the result to the
/// third vector, using NEON SIMD (float version).
/// @details res[i] = a[i] * b[i], i = 0..3.
/// @param a First vector.
/// @param b Second vector.
/// @param res Result vector.
INLINE NOTNULL(1, 2, 3) void real_multiply(
    const float *a, const float *b, float *res) {
  float32x4_t aVec = vld1q_f32(a);
  float32x4_t bVec = vld1q_f32(b);
  float32x4_t resVec = vmulq_f32(aVec, bVec);
  vst1q_f32(res, resVec);
}

/// @brief Performs complex multiplication of the contents of two complex
/// vectors, saving the result to the third vector, using NEON SIMD.
/// @details res[i] = a[i] * b[i] - a[i + 1] * b[i + 1], i = 0, 2;
/// res[i + 1] = a[i] * b[i + 1] + a[i + 1] * b[i], i = 1, 3.
/// @param a First vector.
/// @param b Second vector.
/// @param res Result vector.
INLINE NOTNULL(1, 2, 3) void complex_multiply(
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

/// @brief Multiplies each floating point number in the specified array
/// by the specified value, using NEON SIMD.
/// @details This functions does the same thing as real_multiply_scalar_na, but
/// likely much faster.
/// @param array The array of floating point numbers.
/// @param length The length of the array (in float-s, not in bytes).
/// @param value The value to multiply each number in array.
/// @param res The array to write the results to.
/// @note res must have at least the same length as array.
INLINE NOTNULL(1, 4) void real_multiply_scalar(const float *array,
                                                 size_t length,
                                                 float value, float *res) {
  int ilength = (int)length;
  for (int i = 0; i < ilength - 3; i += 4) {
    float32x4_t vec = vld1q_f32(array + i);
    vec = vmulq_n_f32(vec, value);
    vst1q_f32(res + i, vec);
  }
  for (int i = ((ilength >> 2) << 2); i < ilength; i++) {
    res[i] = array[i] * value;
  }
}

INLINE NOTNULL(1) float sum(const float *input, size_t length) {
  int ilength = (int)length;
  float32x4_t accum = vdupq_n_f32(0.f);
  for (int j = 0; j < ilength - 7; j += 8) {
    float32x4_t vec1 = vld1q_f32(input + j);
    float32x4_t vec2 = vld1q_f32(input + j + 4);
    accum = vaddq_f32(accum, vec1);
    accum = vaddq_f32(accum, vec2);
  }
  float32x2_t accum2 = vpadd_f32(vget_high_f32(accum),
                                 vget_low_f32(accum));
  float res = vget_lane_f32(accum2, 0) + vget_lane_f32(accum2, 1);
  for (int j = ((ilength >> 3) << 3); j < ilength; j++) {
    res += input[j];
  }
  return res;
}

#else

#define int16_to_float int16_to_float_na
#define int32_to_float int32_to_float_na
#define float_to_int16 float_to_int16_na
#define float_to_int32 float_to_int32_na
#define int32_to_int16 int32_to_int16_na
#define int16_to_int32 int16_to_int32_na
#define real_multiply real_multiply_na
#define complex_multiply complex_multiply_na
#define real_multiply_scalar real_multiply_scalar_na
#define sum sum_na

#endif

/*
 * Below are the functions without SIMD instructions.
 */

INLINE int next_highest_power_of_2(int value) {
  value--;
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  return ++value;
}

#endif  // SRC_PRIMITIVES_ARITHMETIC_INL_H_
