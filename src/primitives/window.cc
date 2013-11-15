/*! @file window.c
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


#include "src/primitives/window.h"
#include <cmath>
#include <unordered_map>
#include "src/parameterizable_base.h"

namespace sound_feature_extraction {

constexpr const char* WINDOW_TYPE_RECTANGULAR_STR = "rectangular";
constexpr const char* WINDOW_TYPE_HAMMING_STR = "hamming";
constexpr const char* WINDOW_TYPE_HANNING_STR = "hanning";
constexpr const char* WINDOW_TYPE_HALF_HANNING_RIGHT_STR = "half-hanning-right";
constexpr const char* WINDOW_TYPE_BLACKMAN_STR = "blackman";

WindowType Parse(const std::string& value, identity<WindowType>) {
  static const std::unordered_map<std::string, WindowType> map {
    { WINDOW_TYPE_RECTANGULAR_STR, WINDOW_TYPE_RECTANGULAR },
    { WINDOW_TYPE_HAMMING_STR, WINDOW_TYPE_HAMMING },
    { WINDOW_TYPE_HANNING_STR, WINDOW_TYPE_HANNING },
    { WINDOW_TYPE_HALF_HANNING_RIGHT_STR, WINDOW_TYPE_HALF_HANNING_RIGHT },
    { WINDOW_TYPE_BLACKMAN_STR, WINDOW_TYPE_BLACKMAN }
  };
  auto it = map.find(value);
  if (it == map.end()) {
    throw InvalidParameterValueException();
  }
  return it->second;
}

/// @brief Calculates the element of Hamming window of length "length"
/// at index "index".
/// @note This function conforms to Matlab hamming() function.
static float HammingWindow(int index, int length) {
  return 0.54f - 0.46f * cosf(2 * M_PI * index / (length - 1));
}

static float HanningWindow(int index, int length) {
  return 0.5f - 0.5f * cosf(2 * M_PI * index / (length - 1));
}

static float HalfHanningWindowRight(int index, int length) {
  return HanningWindow(index + length - 1, length * 2 - 1);
}

static float BlackmanWindow(int index, int length) {
  float cosval = 2 * M_PI * index / (length - 1);
  return 0.42f - 0.5f * cosf(cosval) + 0.08f * cosf(2 * cosval);
}

float WindowElement(WindowType type, int length, int index) {
  switch (type) {
    case WINDOW_TYPE_RECTANGULAR:
      return 1.0f;
    case WINDOW_TYPE_HAMMING:
      return HammingWindow(index, length);
    case WINDOW_TYPE_HANNING:
      return HanningWindow(index, length);
    case WINDOW_TYPE_HALF_HANNING_RIGHT:
      return HalfHanningWindowRight(index, length);
    case WINDOW_TYPE_BLACKMAN:
      return BlackmanWindow(index, length);
  }
  return 0.0f;
}

}  // namespace sound_feature_extraction

namespace std {
  string to_string(sound_feature_extraction::WindowType type) noexcept {
    switch (type) {
      case sound_feature_extraction::WINDOW_TYPE_RECTANGULAR:
        return sound_feature_extraction::WINDOW_TYPE_RECTANGULAR_STR;
      case sound_feature_extraction::WINDOW_TYPE_HAMMING:
        return sound_feature_extraction::WINDOW_TYPE_HAMMING_STR;
      case sound_feature_extraction::WINDOW_TYPE_HANNING:
        return sound_feature_extraction::WINDOW_TYPE_HANNING_STR;
      case sound_feature_extraction::WINDOW_TYPE_HALF_HANNING_RIGHT:
        return sound_feature_extraction::WINDOW_TYPE_HALF_HANNING_RIGHT_STR;
      case sound_feature_extraction::WINDOW_TYPE_BLACKMAN:
        return sound_feature_extraction::WINDOW_TYPE_BLACKMAN_STR;
    }
    return "";
  }
}  // namespace std
