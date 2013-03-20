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

#ifndef SRC_PRIMITIVES_WINDOW_H_
#define SRC_PRIMITIVES_WINDOW_H_

#include <string>
#include <unordered_map>  // NOLINT(build/include_order)

/// @brief All implemented window types.
/// @see http://en.wikipedia.org/wiki/Window_function.
typedef enum {
  WINDOW_TYPE_RECTANGULAR,
  WINDOW_TYPE_HAMMING,
  WINDOW_TYPE_HANNING
} WindowType;

/// @brief The map between window name and window type.
/// @details For example,
/// @code
/// WindowTypeMap["hamming"] == WINDOW_TYPE_HAMMING
/// @endcode
/// is true.
extern const std::unordered_map<std::string, WindowType> WindowTypeMap;

/// @brief Returns the window element at the specified index.
/// @param type The window type.
/// @param length The length of the window.
/// @param index The index of the element to return.
/// @return The requested window element.
float WindowElement(WindowType type, int length, int index);

#endif  // SRC_PRIMITIVES_WINDOW_H_
