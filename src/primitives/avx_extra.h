/*! @file avx_extra.h
 *  @brief AVX ElementAt.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_PRIMITIVES_AVX_EXTRA_H_
#define SRC_PRIMITIVES_AVX_EXTRA_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __AVX__

#include <immintrin.h>
#include "src/config.h"

float ElementAt(__m256 vector, int index);

INLINE unsigned long long __xgetbv() {
  unsigned int index = 0;
#if defined(__GNUC__) && __GNUC_PREREQ(4, 4)
  unsigned int eax, edx;
  __asm__ __volatile__("xgetbv" : "=a"(eax), "=d"(edx) : "c"(index));
  return ((unsigned long long)edx << 32) | eax;
#else
  return 0;
#endif
}

#endif  // __AVX__

#ifdef __cplusplus
}
#endif

#endif  // SRC_PRIMITIVES_AVX_EXTRA_H_
