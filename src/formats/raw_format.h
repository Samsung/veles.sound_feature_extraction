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

#include "src/buffers_base.h"
#include "src/exceptions.h"

namespace SpeechFeatureExtraction {
namespace Formats {

class InvalidRawFormatParametersException : public ExceptionBase {
 public:
  InvalidRawFormatParametersException(size_t size, int samplingRate)
  : ExceptionBase("Buffer size " + std::to_string(size) + ", "
                  "sampling rate " + std::to_string(samplingRate) +
                  " are not supported or invalid.") {}
};

struct Raw {
  const int16_t* Data;
};

class RawFormat : public BufferFormatBase<Raw> {
 public:
  RawFormat() noexcept;
  RawFormat(size_t size, int samplingRate);

  BufferFormat& operator=(const BufferFormat& other);

  int SamplingRate() const noexcept;

  /// @brief Returns the raw buffer size in samples.
  size_t Size() const noexcept;

 private:
  size_t size_;
  int samplingRate_;
};

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_RAW_FORMAT_H_
