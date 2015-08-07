/*! @file window.c
 *  @brief Hannig, Half-Hanning, Hamming and Rectangular window types.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 *
 *  @section License
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */


#include "src/primitives/window.h"
#include <cmath>
#include <unordered_map>
#include "src/parameterizable_base.h"

namespace sound_feature_extraction {

constexpr const char* kWindowTypeRectangularStr = "rectangular";
constexpr const char* kWindowTypeHammingStr = "hamming";
constexpr const char* kWindowTypeHanningStr = "hanning";
constexpr const char* kWindowTypeHalfHanningRightStr = "half-hanning-right";
constexpr const char* kWindowTypeBlackmanStr = "blackman";

WindowType Parse(const std::string& value, identity<WindowType>) {
  static const std::unordered_map<std::string, WindowType> map {
    { kWindowTypeRectangularStr, WindowType::kWindowTypeRectangular },
    { kWindowTypeHammingStr, WindowType::kWindowTypeHamming },
    { kWindowTypeHanningStr, WindowType::kWindowTypeHanning },
    { kWindowTypeHalfHanningRightStr, WindowType::kWindowTypeHalfHanningRight },
    { kWindowTypeBlackmanStr, WindowType::kWindowTypeBlackman }
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
    case WindowType::kWindowTypeRectangular:
      return 1.0f;
    case WindowType::kWindowTypeHamming:
      return HammingWindow(index, length);
    case WindowType::kWindowTypeHanning:
      return HanningWindow(index, length);
    case WindowType::kWindowTypeHalfHanningRight:
      return HalfHanningWindowRight(index, length);
    case WindowType::kWindowTypeBlackman:
      return BlackmanWindow(index, length);
  }
  return 0.0f;
}

}  // namespace sound_feature_extraction

namespace std {
  using sound_feature_extraction::WindowType;

  string to_string(sound_feature_extraction::WindowType type) noexcept {
    switch (type) {
      case WindowType::kWindowTypeRectangular:
        return sound_feature_extraction::kWindowTypeRectangularStr;
      case WindowType::kWindowTypeHamming:
        return sound_feature_extraction::kWindowTypeHammingStr;
      case WindowType::kWindowTypeHanning:
        return sound_feature_extraction::kWindowTypeHanningStr;
      case WindowType::kWindowTypeHalfHanningRight:
        return sound_feature_extraction::kWindowTypeHalfHanningRightStr;
      case WindowType::kWindowTypeBlackman:
        return sound_feature_extraction::kWindowTypeBlackmanStr;
    }
    return "";
  }
}  // namespace std
