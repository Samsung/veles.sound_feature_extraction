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

#include "src/config.h"
#include "src/buffer_format.h"
#include "src/buffers.h"
#include "src/demangle.h"

namespace SpeechFeatureExtraction {

template <typename T>
class BufferFormatBase : public BufferFormat {
 public:
  typedef T BufferType;

  BufferFormatBase() noexcept
  : BufferFormat(CutNamespaces(std::demangle(typeid(T).name()))) {
  }

  virtual std::function<void(void*)> Destructor() const noexcept {  // NOLINT(*)
    return [](void* ptr) {  // NOLINT(whitespace/braces)
      auto instance = reinterpret_cast<T*>(ptr);
      delete instance;
    };
  }

  virtual bool MustReallocate(const BufferFormat& other) const noexcept {
    if (*this != other) {
      return true;
    }
    return MustReallocate(
        reinterpret_cast<const BufferFormatBase<T>&>(other));
  }

  virtual size_t PayloadSizeInBytes() const noexcept {
    return 0;
  }

  virtual const void* PayloadPointer(const void* buffer) const noexcept {
    return PayloadPointer(*reinterpret_cast<const T*>(buffer));
  }

 protected:
  std::string CutNamespaces(std::string&& str) {
    return str.substr(str.find_last_of(':') + 1, std::string::npos);
  }

  virtual bool MustReallocate(const BufferFormatBase<T>& other UNUSED)
      const noexcept {
    return true;
  }

  virtual const void* PayloadPointer(const T& item UNUSED) const noexcept {
    return nullptr;
  }
};

template <typename T>
class BuffersBase : public Buffers {
 public:
  explicit BuffersBase() noexcept
      : Buffers(0, std::make_shared<BufferFormatBase<T>>()) {  // NOLINT(*)
  }

  template <typename... TArgs>
  void Initialize(size_t size, TArgs... args) noexcept {
    Buffers::SetSize(size);
    for (size_t i = 0; i < size; i++) {
      Set(i, new T(args...));
    }
  }

  virtual ~BuffersBase() noexcept {
  }

  T* operator[](int index) noexcept {
    return reinterpret_cast<T*>(Buffers::operator [](index));  // NOLINT(*)
  }

  const T* operator[](int index) const noexcept {
    return reinterpret_cast<const T*>(Buffers::operator [](index));  // NOLINT(*)
  }

  virtual const std::shared_ptr<BufferFormat> Format() const noexcept {
    static const std::shared_ptr<BufferFormat> bf =
        std::make_shared<BufferFormatBase<T>>();  // NOLINT(*)
    return bf;
  }
};

}  // namespace SpeechFeatureExtraction

#endif  // SRC_BUFFERS_BASE_H_
