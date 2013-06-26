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
};

}  // namespace Formats
}  // namespace SoundFeatureExtraction

namespace std {
  template<uint8_t L, typename F>
  inline string to_string(
      const SoundFeatureExtraction::Formats::FixedArray<L, F>& __val) {
    std::string res("[");
    for (int i = 0; i < L; i++) {
      res += std::to_string(__val[i]) + ",\t";
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
      *result += std::to_string(std::get<index>(__val)) + ",\t";
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

  SingleFormat(int samplingRate)
      : BufferFormatBase<T>(samplingRate) {
  }

  SingleFormat(const SingleFormat& other) noexcept
      : BufferFormatBase<T>(other) {
  }

  virtual const std::string& Id() const noexcept override final {
    static const std::string id(std::demangle(typeid(T).name()));
    return id;
  }

  BufferFormat& operator=(const BufferFormat& other) {
    if (other.Id() != Id()) {
      throw InvalidFormatException(Id(), other.Id());
    }
    *this = reinterpret_cast<const SingleFormat&>(other);
    return *this;
  }

 protected:
  virtual size_t UnalignedSizeInBytes() const noexcept override {
    return sizeof(T);
  }

  template <class F>
  static typename std::enable_if<std::is_arithmetic<F>::value>::type
  SpecializedValidate(const BuffersBase<F>& buffers,
                      const std::string& id) {
    for (size_t i = 0; i < buffers.Count(); i++) {
      F value = buffers[i];
      if (!Validation::Validator<F>::Validate(value)) {
        throw InvalidBuffersException(id, i,
                                      std::to_string(value));
      }
    }
  }

  template<uint8_t L, typename F = float>
  static void SpecializedValidate(const BuffersBase<FixedArray<L, F>>& buffers,
                                  const std::string& id) {
    for (size_t i = 0; i < buffers.Count(); i++) {
      T value = buffers[i];
      if (!value.Validate()) {
        throw InvalidBuffersException(id, i,
                                      std::to_string(value));
      }
    }
  }


  template<std::size_t I, typename... TP>
  static typename std::enable_if<I == sizeof...(TP), bool>::type
  ValidateTupleElement(const std::tuple<TP...>&) noexcept {
    return true;
  }

  template<std::size_t I = 0, typename... TP>
  static typename std::enable_if<I < sizeof...(TP), bool>::type
  ValidateTupleElement(const std::tuple<TP...>& t) noexcept {
    return Validation::
        Validator<typename std::tuple_element<I, std::tuple<TP...>>::type>::
        Validate(std::get<I>(t)) &
        ValidateTupleElement<I + 1, TP...>(t);
  }


  template<typename... Args>
  static void SpecializedValidate(
      const BuffersBase<std::tuple<Args...>>& buffers,
      const std::string& id) {
    for (size_t i = 0; i < buffers.Count(); i++) {
      const T& value = buffers[i];
      if (!ValidateTupleElement(value)) {
          throw InvalidBuffersException(id, i, std::to_string(value));
      }
    }
  }

  void Validate(const BuffersBase<T>& buffers) const {
    SpecializedValidate(buffers, this->Id());
  }

  virtual std::string Dump(const BuffersBase<T>& buffers)
      const noexcept override {
    std::string ret;
    for (size_t i = 0; i < buffers.Count(); i++) {
      auto indexStr = std::to_string(i);
      indexStr += ":";
      indexStr += std::string(7 - indexStr.size(), ' ');
      ret += indexStr + std::to_string(buffers[i]) + "\n";
    }
    return ret;
  }
};

typedef SingleFormat<float> SingleFormatF;
typedef SingleFormat<int16_t> SingleFormat16;
typedef SingleFormat<int32_t> SingleFormat32;

}  // namespace Formats
}  // namespace SoundFeatureExtraction



#endif  // SRC_FORMATS_SINGLE_FORMAT_H_
