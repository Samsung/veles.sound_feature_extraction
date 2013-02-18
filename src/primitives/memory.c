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
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __AVX__
static int align_offset_internal(void *ptr) {
  uintptr_t addr = (uintptr_t)ptr;
  if ((addr & 31) != 0) {
    return 32 - (addr % 32);
  }
  return 0;
}

int align_offsetf(float *ptr) {
  return align_offset_internal(ptr) / sizeof(float);
}

int align_offset_i16(int16_t *ptr) {
  return align_offset_internal(ptr) / 2;
}
#endif

float *mallocf(size_t length) {
  void *ptr;
  if (posix_memalign(&ptr, 32, length * sizeof(float)) != 0) {
    return NULL;
  }
  return ptr;
}

INLINE void memsetf(float *ptr, size_t length, float value) {
#ifdef __AVX__
  __m256 fillvec = _mm256_set_ps( value, value, value, value,
                                  value, value, value, value );
  size_t startIndex = align_offsetf(ptr);

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
