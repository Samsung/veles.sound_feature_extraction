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

#include <string>
#include <unordered_map>

typedef enum {
  WINDOW_TYPE_RECTANGULAR,
  WINDOW_TYPE_HAMMING
} WindowType;

extern const std::unordered_map<std::string, WindowType> WindowTypeMap;

float WindowElement(WindowType type, int index, int length);

#endif  // INCLUDE_WINDOW_H_
