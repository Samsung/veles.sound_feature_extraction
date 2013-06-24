/*! @file raw_format.h
 *  @brief RawFormat class definition.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FORMATS_RAW_FORMAT_H_
#define SRC_FORMATS_RAW_FORMAT_H_

#include <memory>
#include "src/config.h"
#include "src/buffers_base.h"
#include "src/exceptions.h"
#include <simd/memory.h>

namespace SoundFeatureExtraction {
namespace Formats {

class InvalidRawFormatSizeException : public ExceptionBase {
 public:
  explicit InvalidRawFormatSizeException(size_t size)
  : ExceptionBase("Buffer size " + std::to_string(size) +
                  " is not supported or invalid.") {}
};

template <typename T>
struct Raw {
  std::shared_ptr<T> Data;

#ifdef __AVX__
  explicit Raw(size_t size, int alignmentOffset)
  : Data(reinterpret_cast<T*>(malloc_aligned_offset(
      size * sizeof(T), alignmentOffset)),
      [=](T *ptr) {
        free(reinterpret_cast<char*>(ptr) - alignmentOffset);
      }
    ) {
  }
#else
  explicit Raw(size_t size, int alignmentOffset UNUSED)
  : Data(reinterpret_cast<T*>(
      malloc_aligned(size * sizeof(T))),
      [](T *ptr) { free(ptr); }) {
  }
#endif

  Raw(const Raw& other) : Data(other.Data) {
  }

  explicit Raw(T* ptr) : Data(ptr, [](T*) {}) {
  }

  Raw& operator=(const Raw& other) {
    Data = other.Data;
    return *this;
  }

  int AlignmentOffset() const {
#ifdef __AVX__
    return reinterpret_cast<uintptr_t>(Data.get()) % 32;
#else
    return 0;
#endif
  }

  T& operator[](int index) {
    return Data.get()[index];
  }

  const T& operator[](int index) const {
    return Data.get()[index];
  }
};

typedef Raw<int16_t> Raw16;
typedef Raw<int32_t> Raw32;
typedef Raw<float> RawF;

template <typename T>
class RawFormat : public BufferFormatBase<Raw<T>> {
 public:
  typedef T BufferElementType;

  RawFormat() noexcept
      : size_(0) {
  }

  RawFormat(size_t size, int samplingRate)
      : BufferFormatBase<Raw<T>>(samplingRate),
        size_(size) {
    ValidateSize(size_);
  }

  BufferFormat& operator=(const BufferFormat& other) {
    if (other.Id() != RawFormat<T>::Id()) {
      throw InvalidFormatException(RawFormat<T>::Id(), other.Id());
    }
    *this = reinterpret_cast<const RawFormat&>(other);
    return *this;
  }

  /// @brief Returns the raw buffer size in samples.
  size_t Size() const noexcept {
    return size_;
  }

  void SetSize(size_t value) {
    ValidateSize(value);
    size_ = value;
  }

  virtual size_t PayloadSizeInBytes() const noexcept {
    return size_ * sizeof(T);
  }

  static const size_t MIN_RAW_SIZE = 128;
  static const size_t MAX_RAW_SIZE = (1 << 30);

 protected:
  virtual bool MustReallocate(const BufferFormatBase<Raw<T>>& other)
      const noexcept{  // NOLINT(*)
    auto inst = reinterpret_cast<const RawFormat<T>&>(other);
    return inst.size_ < size_;
  }

  virtual const void* PayloadPointer(const Raw<T>& item) const noexcept {
    return item.Data.get();
  }

  virtual void Validate(const BuffersBase<Raw<T>>& buffers) const {
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

  virtual std::string Dump(const BuffersBase<Raw<T>>& buffers UNUSED) const noexcept {
    return "TODO";
  }

 private:
  size_t size_;
  int samplingRate_;

  static void ValidateSize(size_t value) {
    if (value < MIN_RAW_SIZE || value > MAX_RAW_SIZE) {
      throw InvalidRawFormatSizeException(value);
    }
  }
};

typedef RawFormat<int16_t> RawFormat16;
typedef RawFormat<int32_t> RawFormat32;
typedef RawFormat<float> RawFormatF;

}  // namespace Formats
}  // namespace SoundFeatureExtraction
#endif  // SRC_FORMATS_RAW_FORMAT_H_
