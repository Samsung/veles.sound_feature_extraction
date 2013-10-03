/*! @file fixed_array.h
 *  @brief Array with compile-time size.
 *  @author Vadim Markovtsev <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FORMATS_FIXED_ARRAY_H_
#define SRC_FORMATS_FIXED_ARRAY_H_

#include <cstdint>
#include <stdexcept>
#include "src/buffers_base.h"

namespace SoundFeatureExtraction {
namespace Formats {

template<std::uint8_t L, typename F = float>
struct FixedArray {
  static_assert(std::is_arithmetic<F>(), "F must be an arithmetic type");
  F Data[L];

  F& operator[](int index) {
    if (index < 0 || index > L - 1) {
      throw std::out_of_range("index");
    }
    return Data[index];
  }

  const F& operator[](int index) const {
    if (index < 0 || index > L - 1) {
      throw std::out_of_range("index");
    }
    return Data[index];
  }

  bool Validate() const noexcept {
    for (int i = 0; i < L; i++) {
      if (!Validation::Validator<F>::Validate(this->Data[i])) {
        return false;
      }
    }
    return true;
  }

  bool operator== (F value) const noexcept {
    for (int i = 0; i < L; i++) {
      if (Data[i] != value) {
        return false;
      }
    }
    return true;
  }
};

}  // namespace Formats
}  // namespace SoundFeatureExtraction

namespace std {
  template<std::uint8_t L, typename F>
  inline string to_string(
      const SoundFeatureExtraction::Formats::FixedArray<L, F>& __val) {
    std::string res("[");
    for (int i = 0; i < L; i++) {
      res += std::to_string(__val[i]) + ",\t";
    }
    res = res.substr(0, res.size() - 2);
    res += "]";
    return res;
  }
}  // namespace std

#endif  // SRC_FORMATS_FIXED_ARRAY_H_
