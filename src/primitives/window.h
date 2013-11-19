/*! @file window.h
 *  @brief Hannig, Half-Hanning, Hamming and Rectangular window types.
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
#include "src/parameterizable_base.h"

namespace sound_feature_extraction {

/// @brief All implemented window types.
/// @see http://en.wikipedia.org/wiki/Window_function.
enum class WindowType {
  kWindowTypeRectangular,
  kWindowTypeHamming,
  kWindowTypeHanning,
  kWindowTypeHalfHanningRight,
  kWindowTypeBlackman
};

WindowType Parse(const std::string& value, identity<WindowType>);

/// @brief Returns the window element at the specified index.
/// @param type The window type.
/// @param length The length of the window.
/// @param index The index of the element to return.
/// @return The requested window element.
float WindowElement(WindowType type, int length, int index);

}  // namespace sound_feature_extraction

namespace std {
  string to_string(sound_feature_extraction::WindowType type) noexcept;
}  // namespace std

#endif  // SRC_PRIMITIVES_WINDOW_H_
