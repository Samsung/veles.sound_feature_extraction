/*! @file window_format.h
 *  @brief Window frame format.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FORMATS_WINDOW_FORMAT_H_
#define SRC_FORMATS_WINDOW_FORMAT_H_

#include "src/attributes.h"
#include "src/buffers_base.h"
#include <simd/memory.h>

namespace SoundFeatureExtraction {
namespace Formats {

class InvalidWindowDurationException : public ExceptionBase {
 public:
  explicit InvalidWindowDurationException(float duration)
  : ExceptionBase("Duration " + std::to_string(duration) +
                  " is not supported or invalid.") {}
};

class TooBigWindowSizeException : public ExceptionBase {
 public:
  explicit TooBigWindowSizeException(size_t newSize,
                                     size_t allocated)
  : ExceptionBase("Attempted to set window size to " +
                  std::to_string(newSize) +
                  " while only " + std::to_string(allocated) +
                  " units were allocated.") {}
};

class TooSmallWindowAllocatedSizeException : public ExceptionBase {
 public:
  explicit TooSmallWindowAllocatedSizeException(size_t newAllocatedSize,
                                                size_t size)
  : ExceptionBase("Attempted to set allocated size to " +
                  std::to_string(newAllocatedSize) +
                  " while window size was " + std::to_string(size) +
                  " units.") {}
};

template <typename T>
struct Window {
  std::shared_ptr<T> Data;

  explicit Window(size_t size)
  : Data(reinterpret_cast<T*>(
      malloc_aligned(size * sizeof(T))),
      [](T *ptr) { free(ptr); }) {
  }

  Window(const Window& other) : Data(other.Data) {
  }

  explicit Window(T* ptr) : Data(ptr, [](T*) {}) {
  }

  Window& operator=(const Window& other) {
    Data = other.Data;
    return *this;
  }

  T& operator[](int index) {
    return Data.get()[index];
  }

  const T& operator[](int index) const {
    return Data.get()[index];
  }
};

typedef Window<int16_t> Window16;
typedef Window<int32_t> Window32;
typedef Window<float> WindowF;

template <class T>
class WindowFormat
    : public BufferFormatBase<Window<T>>,
      public Attributes
{
 public:
  typedef T BufferElementType;

  WindowFormat() noexcept
      : size_(0),
        allocatedSize_(size_),
        parentRawSize_(0) {
  }

  WindowFormat(size_t size, int samplingRate, size_t parentRawSize = 0)
      : BufferFormatBase<Window<T>>(samplingRate),
        size_(size),
        allocatedSize_(size_),
        parentRawSize_(parentRawSize) {
  }

  WindowFormat(const WindowFormat& other) noexcept
      : BufferFormatBase<Window<T>>(other),
        size_(other.size_),
        allocatedSize_(other.allocatedSize_),
        parentRawSize_(other.parentRawSize_) {
  }

  BufferFormat& operator=(const BufferFormat& other) {
    if (other.Id() != WindowFormat<T>::Id()) {
      throw InvalidFormatException(WindowFormat<T>::Id(), other.Id());
    }
    *this = reinterpret_cast<const WindowFormat&>(other);
    return *this;
  }

  float Duration() const noexcept {
    return size_ * 2.f / this->SamplingRate();
  }

  void SetDuration(float value) {
    ValidateDuration(value);
    size_ = value * this->SamplingRate() / 2;
    if (size_ > allocatedSize_) {
      throw TooBigWindowSizeException(size_, allocatedSize_);
    }
  }

  /// @brief Returns the current buffer size in data units (sizeof(T)).
  size_t Size() const noexcept {
    return size_;
  }

  void SetSize(size_t value) {
    if (value > allocatedSize_) {
      throw TooBigWindowSizeException(value, allocatedSize_);
    }
    size_ = value;
  }

  virtual size_t PayloadSizeInBytes() const noexcept {
    return size_ * sizeof(T);
  }

  /// @brief Returns the allocated buffer size in data units (sizeof(T)).
  size_t AllocatedSize() const noexcept {
    return allocatedSize_;
  }

  void SetAllocatedSize(size_t value) {
    if (size_ > value) {
      throw TooSmallWindowAllocatedSizeException(value, size_);
    }
    allocatedSize_ = value;
  }

  /// @brief Returns the parent Raw (that is, before cutting into windows)
  /// data size.
  size_t ParentRawSize() const noexcept {
    return parentRawSize_;
  }

  void SetParentRawSize(size_t value) noexcept {
     parentRawSize_ = value;
  }

  static const int MIN_WINDOW_SAMPLES = 32;
  static const int MAX_WINDOW_SAMPLES = 8096;
  static const int DEFAULT_WINDOW_SAMPLES = 512;
  static const int DEFAULT_WINDOW_STEP = 205;

 protected:
  virtual bool MustReallocate(const BufferFormatBase<Window<T>>& other)
      const noexcept {
    auto inst = reinterpret_cast<const WindowFormat<T>&>(other);
    return inst.allocatedSize_ < allocatedSize_;
  }

  virtual const void* PayloadPointer(const Window<T>& item) const noexcept {
    return item.Data.get();
  }

  virtual void Validate(const BuffersBase<Window<T>>& buffers) const {
    for (size_t i = 0; i < buffers.Size(); i++) {
      bool allZeros = true;
      for (size_t j = 0; j < size_; j++) {
        T value = buffers[i].Data.get()[j];
        if (!Validation::Validator<T>::Validate(value)) {
          throw InvalidBuffersException(this->Id(), i,
                                        std::string("[") + std::to_string(j) +
                                        "] = " + std::to_string(value));
        }
        allZeros &= (value == 0);
      }
      if (allZeros) {
        throw InvalidBuffersException(this->Id(), i, "all zeros");
      }
    }
  }

  virtual std::string Dump(const BuffersBase<Window<T>>& buffers) const noexcept {
    std::string ret("Length each: ");
    ret += std::to_string(size_) + "\n";
    ret += "Allocated size: " + std::to_string(allocatedSize_) + "\n";
    for (size_t i = 0; i < buffers.Size(); i++) {
      ret += "----" + std::to_string(i) + "----\n";
      for (size_t j = 0; j < size_; j++) {
        auto strval = std::to_string(buffers[i].Data.get()[j]);
        size_t sizeLimit = 16;
        if (strval[0] != '-') {
          ret += ' ';
          sizeLimit--;
        }
        ret += strval + std::string(sizeLimit - strval.size(), ' ');
        if (((j + 1) % 10) == 0) {
          ret += "\n";
        }
      }
      ret += "\n";
    }
    return ret;
  }

  static constexpr size_t MIN_WINDOW_DURATION() {
    return 1 + MIN_WINDOW_SAMPLES * 1000 /
        BufferFormatBase<Window<T>>::MAX_SAMPLING_RATE;
  }

  static constexpr size_t MAX_WINDOW_DURATION() {
    return MAX_WINDOW_SAMPLES * 1000 /
        BufferFormatBase<Window<T>>::MIN_SAMPLING_RATE;
  }

 private:
  size_t size_;
  size_t allocatedSize_;
  size_t parentRawSize_;

  static void ValidateDuration(size_t value) {
    if (value < MIN_WINDOW_DURATION() || value > MAX_WINDOW_DURATION()) {
      throw InvalidWindowDurationException(value);
    }
  }
};

typedef WindowFormat<int16_t> WindowFormat16;
typedef WindowFormat<int32_t> WindowFormat32;
typedef WindowFormat<float> WindowFormatF;

}  // namespace Formats
}  // namespace SoundFeatureExtraction
#endif  // SRC_FORMATS_WINDOW_FORMAT_H_
