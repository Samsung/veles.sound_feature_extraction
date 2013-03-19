/*! @file symlets.h
 *  @brief Coefficients of Symlet wavelets.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_PRIMITIVES_SYMLETS_H_
#define SRC_PRIMITIVES_SYMLETS_H_

#ifndef WAVELET_INTERNAL_USE
#error This file should not be included directly into mid level code
#endif

extern const double SymletsD[38][76];
extern const float  SymletsF[38][76];

#endif  // SRC_PRIMITIVES_SYMLETS_H_
