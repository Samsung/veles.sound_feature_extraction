/*! @file avx_extra.h
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef AVX_EXTRA_H_
#define AVX_EXTRA_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __AVX__

#include "src/config.h"

#include <immintrin.h>

float ElementAt(__m256 vector, int index);

#endif  // __AVX__

#ifdef __cplusplus
}
#endif

#endif  // AVX_EXTRA_H_
