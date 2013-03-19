/*! @file wavelet_types.h
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

#ifndef SRC_PRIMITIVES_WAVELET_TYPES_H_
#define SRC_PRIMITIVES_WAVELET_TYPES_H_

typedef enum {
  WAVELET_TYPE_DAUBECHIES,
  WAVELET_TYPE_COIFLET,
  WAVELET_TYPE_SYMLET
} WaveletType;

#endif  // SRC_PRIMITIVES_WAVELET_TYPES_H_
