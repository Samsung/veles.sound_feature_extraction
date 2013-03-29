/*! @file avx_extra.c
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

#include "src/primitives/avx_extra.h"

#ifdef __AVX__
float ElementAt(__m256 vector, int index) {
  return vector[index];
}
#endif

