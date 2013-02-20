/*! @file memory.h
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

#ifndef MEMORY_H_
#define MEMORY_H_

#include <string.h>
#include "spfextr/config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __AVX__
#include <stdint.h>

int align_complement_f32(const float *ptr) NOTNULL((1));
int align_complement_i16(const int16_t *ptr) NOTNULL((1));
int align_complement_i32(const int32_t *ptr) NOTNULL((1));

void *malloc_aligned(size_t size) MALLOC;
void *malloc_aligned_offset(size_t size, int offset) MALLOC;
#endif

float *mallocf(size_t length) MALLOC;

void memsetf(float *ptr, size_t length, float value) NOTNULL((1));

float *zeropadding(const float *ptr, size_t length, size_t *newLength)
NOTNULL((1, 3)) MALLOC;

float *zeropaddingex(const float *ptr, size_t length, size_t *newLength,
                     size_t additionalLength)
NOTNULL((1, 3)) MALLOC;

#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_MEMORY_H_
