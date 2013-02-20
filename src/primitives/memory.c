/*! @file memory.cc
 *  @brief Memory routines with SIMD optimization.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/primitives/memory.h"
#include <assert.h>
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include <stdlib.h>
#include <stdint.h>

#ifdef __AVX__
static int align_offset_internal(const void *ptr) {
  uintptr_t addr = (uintptr_t)ptr;
  if ((addr & 31) != 0) {
    return 32 - (addr % 32);
  }
  return 0;
}

int align_complement_f32(const float *ptr) {
  return align_offset_internal(ptr) / 4;
}

int align_complement_i16(const int16_t *ptr) {
  return align_offset_internal(ptr) / 2;
}

int align_complement_i32(const int32_t *ptr) {
  return align_offset_internal(ptr) /4;
}

void *malloc_aligned_offset(size_t size, int offset) {
  assert(offset >= 0 && offset < 32);
  void *ptr = malloc_aligned(size + offset);
  return (char *)ptr + offset;
}
#endif

void *malloc_aligned(size_t size) {
  void *ptr;
  if (posix_memalign(&ptr,
#ifdef __AVX__
                     32,
#elif defined(__ARM_NEON__)
                     64,
#else
                     1,
#endif
                     size) != 0) {
    return NULL;
  }
  return ptr;
}

float *mallocf(size_t length) {
  return malloc_aligned(length * sizeof(float));
}

INLINE void memsetf(float *ptr, size_t length, float value) {
#ifdef __AVX__
  __m256 fillvec = _mm256_set_ps( value, value, value, value,
                                  value, value, value, value );
  size_t startIndex = align_complement_f32(ptr);

  for (size_t i = 0; i < startIndex; i++) {
    ptr[i] = value;
  }

  for (int i = (int)startIndex; i < (int)length - 7; i += 8) {
    _mm256_store_ps(ptr + i, fillvec);
  }

  for (size_t i = startIndex + (((length - startIndex) >> 3) << 3);
      i < length; i++) {
    ptr[i] = value;
  }
#elif defined(__ARM_NEON__)
  const float32x4_t fillvec = { value, value, value, value };
  for (size_t i = 0; i < length - 3; i += 4) {
    vst1q_f32(ptr + i, fillvec);
  }
  for (size_t i = ((length >> 2) << 2); i < length; i++) {
    ptr[i] = value;
  }
#else
  for (size_t i = 0; i < length; i++) {
    ptr[i] = value;
  }
#endif
}

float *zeropadding(const float *ptr, size_t length, size_t *newLength) {
  return zeropaddingex(ptr, length, newLength, 0);
}

float *zeropaddingex(const float *ptr, size_t length, size_t *newLength,
                     size_t additionalLength) {
  size_t nl = length;
  int log = 0;
  while (nl >>= 1) {
    log++;
  }
  nl = (1 << log);
  nl <<= 2;
  *newLength = nl;
  float *ret = mallocf(nl + additionalLength);
  memcpy(ret, ptr, length * sizeof(float));
  memsetf(ret + length, nl - length, .0f);
  return ret;
}
