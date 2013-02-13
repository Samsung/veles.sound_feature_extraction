/*! @file convolute.h
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

#ifndef CONVOLUTE_H_
#define CONVOLUTE_H_

#include <stddef.h>
#include "spfextr/config.h"

#ifdef __cplusplus
extern "C" {
#endif

void convolute(const float *__restrict x, size_t xLength,
               const float *__restrict h, size_t hLength,
               float *__restrict result) NOTNULL((1, 3, 5));

#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_CONVOLUTE_H_
