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

#include <stddef.h>
#include "spfextr/config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __AVX__
#include <stdint.h>

int align_offsetf(float *ptr) NOTNULL((1));
int align_offset_i16(int16_t *ptr) NOTNULL((1));
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
