/*! @file raw_format.h
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef RAW_FORMAT_H_
#define RAW_FORMAT_H_

#include <memory>
#include "src/buffers_base.h"
#include "src/exceptions.h"

namespace SpeechFeatureExtraction {
namespace Formats {

class InvalidRawFormatSizeException : public ExceptionBase {
 public:
  InvalidRawFormatSizeException(size_t size)
  : ExceptionBase("Buffer size " + std::to_string(size) +
                  " is not supported or invalid.") {}
};

class InvalidRawFormatSamplingRateException : public ExceptionBase {
 public:
  InvalidRawFormatSamplingRateException(int samplingRate)
  : ExceptionBase("Sampling rate " + std::to_string(samplingRate) +
                  " is not supported or invalid.") {}
};

struct Raw {
  std::shared_ptr<int16_t> Data;

  Raw() : Data(nullptr) {
  }

  explicit Raw(size_t size)
  : Data(new int16_t[size],
         [](int16_t *ptr) { delete[] ptr; }) {
  }

  Raw(const Raw& other) : Data(other.Data) {
  }

  explicit Raw(int16_t *ptr) : Data(ptr, [](int16_t *){}) {
  }

  Raw& operator=(const Raw& other) {
    Data = other.Data;
    return *this;
  }
};

class RawFormat : public BufferFormatBase<Raw> {
 public:
  RawFormat() noexcept;
  RawFormat(const RawFormat& other) noexcept;
  RawFormat(size_t size, int samplingRate);

  BufferFormat& operator=(const BufferFormat& other);

  int SamplingRate() const noexcept;
  void SetSamplingRate(int value);

  /// @brief Returns the raw buffer size in samples.
  size_t Size() const noexcept;
  void SetSize(size_t value);

 private:
  size_t size_;
  int samplingRate_;

  static void ValidateSize(size_t value);
  static void ValidateSamplingRate(int value);
};

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_RAW_FORMAT_H_
