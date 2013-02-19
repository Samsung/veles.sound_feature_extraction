/*! @file window.h
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

#ifndef WINDOW_H_
#define WINDOW_H_

#include <math.h>
#include <spfextr/config.h>

/// @brief Calculates the element of Hamming window of length "length"
/// at index "index".
/// @note This function conforms to Matlab hamming() function.
INLINE float HammingWindow(int index, int length) {
  return 0.54f - 0.46f * cosf(2 * M_PI * index / (length - 1));
}

#endif  // INCLUDE_WINDOW_H_
