/*! @file raw_format.h
 *  @brief Raw frames format.
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

#include <simd/memory.h>
#include "src/buffers_base.h"

namespace SoundFeatureExtraction {
namespace Formats {

class InvalidDurationException : public ExceptionBase {
 public:
  explicit InvalidDurationException(float duration)
  : ExceptionBase("Duration " + std::to_string(duration) +
                  " is not supported or invalid.") {}
};

template <class T>
class RawFormat
    : public BufferFormatBase<T*>,
      public FormatLogger<RawFormat<T>>
{
 public:
  typedef T BufferElementType;

  RawFormat()
      : size_(0) {
  }

  RawFormat(size_t size, int samplingRate)
      : BufferFormatBase<T*>(samplingRate),
        size_(size) {
  }

  RawFormat(const RawFormat& other)
      : BufferFormatBase<T*>(other),
        size_(other.size_) {
  }

  virtual ~RawFormat() noexcept {
  }

  BufferFormat& operator=(const BufferFormat& other) {
    if (other.Id() != RawFormat<T>::Id()) {
      throw InvalidFormatException(RawFormat<T>::Id(), other.Id());
    }
    *this = reinterpret_cast<const RawFormat&>(other);
    return *this;
  }

  float Duration() const noexcept {
    return size_ * 2.f / this->SamplingRate();
  }

  void SetDuration(float value) {
    ValidateDuration(value);
    size_ = value * this->SamplingRate() / 2;
  }

  /// @brief Returns the current buffer size in data units (sizeof(T)).
  size_t Size() const noexcept {
    return size_;
  }

  void SetSize(size_t value) {
    size_ = value;
  }

  static const int kMinSamplesCount = 32;
  static const int kMaxSamplesCount = (1 << 30);

 protected:
  virtual void Validate(const BuffersBase<T*>& buffers) const {
    for (size_t i = 0; i < buffers.Count(); i++) {
      bool allZeros = true;
      for (size_t j = 0; j < size_; j++) {
        T value = buffers[i][j];
        if (!Validation::Validator<T>::Validate(value)) {
          throw InvalidBuffersException(this->Id(), i,
                                        std::string("[") + std::to_string(j) +
                                        "] = " + std::to_string(value));
        }
        allZeros &= (value == 0);
      }
      if (allZeros) {
        WRN("%s", InvalidBuffersException(this->Id(), i, "all zeros").what());
      }
    }
  }

  virtual std::string Dump(const BuffersBase<T*>& buffers, size_t index)
      const noexcept override {
    std::string ret;
    for (size_t j = 0; j < size_; j++) {
      auto strval = std::to_string(buffers[index][j]);
      int size_limit = 24;
      if (strval[0] != '-') {
        ret += ' ';
        size_limit--;
      }
      int skip_size = size_limit - strval.size();
      if (skip_size > 0) {
        ret += strval + std::string(skip_size, ' ');
      } else {
        ret += strval;
      }
      if (((j + 1) % 10) == 0) {
        ret += "\n";
      }
    }
    ret += "----------------\n";
    return ret;
  }

  virtual std::string ToString() const noexcept override {
    return this->Id() + " of size " + std::to_string(Size());
  }

  virtual size_t UnalignedSizeInBytes() const noexcept override {
    return size_ * sizeof(T);
  }

  static constexpr size_t MIN_DURATION() {
    return 1 + kMinSamplesCount * 1000 /
        BufferFormatBase<T*>::MAX_SAMPLING_RATE;
  }

  static constexpr size_t MAX_DURATION() {
    return kMaxSamplesCount * 1000 /
        BufferFormatBase<T*>::MIN_SAMPLING_RATE;
  }

 private:
  size_t size_;

  static void ValidateDuration(size_t value) {
    if (value < MIN_DURATION() || value > MAX_DURATION()) {
      throw InvalidDurationException(value);
    }
  }
};

typedef RawFormat<int16_t> RawFormat16;
typedef RawFormat<int32_t> RawFormat32;
typedef RawFormat<float> RawFormatF;

}  // namespace Formats
}  // namespace SoundFeatureExtraction
#endif  // SRC_FORMATS_RAW_FORMAT_H_
