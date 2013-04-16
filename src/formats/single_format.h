/*! @file single_format.h
 *  @brief Single number format.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FORMATS_SINGLE_FORMAT_H_
#define SRC_FORMATS_SINGLE_FORMAT_H_

#include <stdexcept>
#include <tuple>
#include "src/buffers_base.h"

namespace SoundFeatureExtraction {
namespace Formats {

template<uint8_t L, typename F = float>
struct FixedArray {
  static_assert(std::is_arithmetic<F>(), "F must be an arithmetic type");
  F Data[L];

  F& operator[](int index) {
    if (index < 0 || index > L - 1) {
      throw std::out_of_range("index");
    }
    return Data[index];
  }

  F operator[](int index) const {
    if (index < 0 || index > L - 1) {
      throw std::out_of_range("index");
    }
    return Data[index];
  }

  bool operator!=(const FixedArray& other) {
    for (int i = 0; i < L; i++) {
      if (Data[i] != other[i]) {
        return true;
      }
    }
    return false;
  }
};

}  // namespace Formats
}  // namespace SoundFeatureExtraction

namespace std {
  template<uint8_t L, typename F>
  inline string to_string(
      const SoundFeatureExtraction::Formats::FixedArray<L, F>& __val) {
    std::string res("[");
    for (int i = 0; i < L; i++) {
      res += std::to_string(__val[i]) + ", ";
    }
    res = res.substr(0, res.size() - 2);
    res += "]";
    return std::move(res);
  }

  template<size_t index, typename... Args>
  inline void to_string(int realIndex,
                        const std::tuple<Args...>& __val,
                        std::string* result) {
    if (realIndex == index) {
      *result += std::to_string(std::get<index>(__val)) + ", ";
      to_string<index + 1 < sizeof...(Args)? index + 1 : 0>(index + 1,
                                                            __val, result);
    }
  }

  template<typename... Args>
  inline string to_string(const std::tuple<Args...>& __val) {
    if (sizeof...(Args) > 0) {
      std::string res("[");
      to_string<0>(0, __val, &res);
      res = res.substr(0, res.size() - 2);
      res += "]";
      return std::move(res);
    }
    return "[]";
  }
}  // namespace std

namespace SoundFeatureExtraction {
namespace Formats {

template<class T>
class SingleFormat : public BufferFormatBase<T> {
 public:
  SingleFormat() noexcept {
  }

  SingleFormat(const SingleFormat& other UNUSED) noexcept {
  }

  virtual const std::string& Id() const noexcept {
    static const std::string id("Number");
    return id;
  }

  BufferFormat& operator=(const BufferFormat& other) {
    if (other.Id() != Id()) {
      throw InvalidFormatException(Id(), other.Id());
    }
    *this = reinterpret_cast<const SingleFormat&>(other);
    return *this;
  }

  virtual size_t PayloadSizeInBytes() const noexcept {
    return sizeof(T);
  }

 protected:
  virtual bool MustReallocate(const BufferFormatBase<T>& other UNUSED)
      const noexcept {
    return false;
  }

  virtual const void* PayloadPointer(const T& item) const noexcept {
    return &item;
  }

  virtual void Validate(const BuffersBase<T>& buffers) const {
    for (size_t i = 0; i < buffers.Size(); i++) {
      T value = *buffers[i];
      if (value != value) {
        throw InvalidBuffersException(this->Id(), i,
                                      std::to_string(value));
      }
    }
  }

  virtual std::string Dump(const BuffersBase<T>& buffers) const noexcept {
    std::string ret;
    for (size_t i = 0; i < buffers.Size(); i++) {
      ret += std::to_string(i) + ":\t" + std::to_string(*buffers[i]) + "\n";
    }
    return ret;
  }
};

typedef SingleFormat<float> SingleFormatF;
typedef SingleFormat<int16_t> SingleFormat16;
typedef SingleFormat<int32_t> SingleForma32;

}  // namespace Formats
}  // namespace SoundFeatureExtraction



#endif  // SRC_FORMATS_SINGLE_FORMAT_H_
