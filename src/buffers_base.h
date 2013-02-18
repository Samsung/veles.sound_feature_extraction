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

#ifndef BUFFERS_BASE_H_
#define BUFFERS_BASE_H_

#include "src/buffer_format.h"
#include "src/buffers.h"

namespace SpeechFeatureExtraction {

template <typename T>
class BufferFormatBase : public BufferFormat {
 public:
  typedef T BufferType;

  BufferFormatBase() noexcept
  : BufferFormat(typeid(T).name()) {
  }

  virtual std::function<void(void*)> Destructor() const noexcept {
    return [](void* ptr) {
      auto instance = reinterpret_cast<T*>(ptr);
      delete instance;
    };
  }
};

template <typename T>
class BuffersBase : public Buffers {
 public:
  explicit BuffersBase() noexcept
  : Buffers(0, BufferFormatBase<T>()) {
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
    return reinterpret_cast<T*>(Buffers::operator [](index));
  }

  const T* operator[](int index) const noexcept {
    return reinterpret_cast<const T*>(Buffers::operator [](index));
  }

  const BufferFormat& Format() const noexcept {
    static const BufferFormatBase<T> bf;
    return bf;
  }
};

}  // namespace SpeechFeatureExtraction

#endif  // INCLUDE_BUFFERS_BASE_H_
