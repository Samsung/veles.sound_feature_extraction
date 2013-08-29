/*! @file buffers_base.h
 *  @brief Base class for buffer formats which implements BufferFormat.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_BUFFERS_BASE_H_
#define SRC_BUFFERS_BASE_H_

#include <assert.h>
#include <limits>
#include "src/config.h"
#include "src/buffer_format.h"
#include "src/buffers.h"
#include "src/demangle.h"
#include "src/logger.h"

namespace SoundFeatureExtraction {

class InvalidBuffersException : public ExceptionBase {
 public:
  InvalidBuffersException(const std::string& format,
                          size_t index, const std::string& value)
  : ExceptionBase("Buffers[" + std::to_string(index) +
                  "] is invalid (" + value + "). Format is " +
                  format + ".") {}
};

template <typename T>
class BuffersBase;

template <typename T>
class BufferFormatBase : public BufferFormat {
 public:
  typedef T BufferType;

  BufferFormatBase()
      : BufferFormat(CutNamespaces(std::demangle(typeid(T).name()))) {
  }

  BufferFormatBase(int samplingRate)
      : BufferFormat(CutNamespaces(std::demangle(typeid(T).name())),
                     samplingRate) {
  }

  virtual void Validate(const Buffers& buffers) const override final {
    if (*this != *buffers.Format()) {
      throw InvalidFormatException(Id(), buffers.Format()->Id());
    }
    Validate(reinterpret_cast<const BuffersBase<T>&>(buffers));
  }

  virtual std::string Dump(const Buffers& buffers)
      const noexcept override final {
    if (*this != *buffers.Format()) {
      throw InvalidFormatException(Id(), buffers.Format()->Id());
    }
    std::string ret("Buffers count: ");
    ret += std::to_string(buffers.Count());
    ret += "\n";
    ret += Dump(reinterpret_cast<const BuffersBase<T>&>(buffers));
    return ret;
  }

 protected:
  std::string CutNamespaces(std::string&& str) {
    return str.substr(str.find_last_of(':') + 1, std::string::npos);
  }

  virtual void Validate(const BuffersBase<T>& buffers) const = 0;

  virtual std::string Dump(const BuffersBase<T>& buffers) const noexcept = 0;
};

namespace Validation {
  template <class TE>
  struct Validator {
    static bool Validate(const TE&) noexcept {
      return true;
    }

    static bool Validate(TE&&) noexcept {
      return true;
    }
  };

  template <>
  struct Validator<float> {
    static bool Validate(float value) noexcept {
      return value == value && value != std::numeric_limits<float>::infinity() &&
        value != -std::numeric_limits<float>::infinity();
    }
  };
}  // namespace Validation

template <typename T>
class BuffersBase : public Buffers {
 public:
  typedef T ElementType;

  BuffersBase(const std::shared_ptr<BufferFormatBase<T>>& format,
              size_t count, void* reusedMemory = nullptr)
      : Buffers(format, count, reusedMemory) {
  }

  T& operator[](int index) noexcept {
    return *reinterpret_cast<T*>(Buffers::operator [](index));  // NOLINT(*)
  }

  const T& operator[](int index) const noexcept {
    return *reinterpret_cast<const T*>(Buffers::operator [](index));  // NOLINT(*)
  }

  const std::shared_ptr<BufferFormatBase<T>> Format() const noexcept {
    return std::static_pointer_cast<BufferFormatBase<T>>(Buffers::Format());
  }

private:
  BuffersBase(const BuffersBase<T>&) = delete;
  BuffersBase& operator=(const BuffersBase<T>&) = delete;

  void RunConstructors() {
    if (!std::is_pointer<T>::value) {
      for (size_t i = 0; i < this->Count(); i++) {
        auto mem = reinterpret_cast<T *>(Buffers::operator [](i));
        new(mem) T();
      }
    }
  }
};

template <typename T>
class BuffersBase<T*> : public Buffers {
 public:
  typedef T* ElementType;

  BuffersBase(const std::shared_ptr<BufferFormatBase<T*>>& format,
              size_t count, void* reusedMemory = nullptr)
      : Buffers(format, count, reusedMemory) {
  }

  T* operator[](int index) noexcept {
    return reinterpret_cast<T*>(Buffers::operator [](index));  // NOLINT(*)
  }

  const T* operator[](int index) const noexcept {
    return reinterpret_cast<const T*>(Buffers::operator [](index));  // NOLINT(*)
  }

  const std::shared_ptr<BufferFormatBase<T*>> Format() const noexcept {
    return std::static_pointer_cast<BufferFormatBase<T*>>(Buffers::Format());
  }

private:
  BuffersBase(const BuffersBase<T*>&) = delete;
  BuffersBase& operator=(const BuffersBase<T*>&) = delete;
};

template<class T>
class FormatLogger : public Logger {
 public:
  FormatLogger() : Logger(std::demangle(typeid(T).name()), EINA_COLOR_ORANGE) {
  }
};

}  // namespace SoundFeatureExtraction

#endif  // SRC_BUFFERS_BASE_H_
