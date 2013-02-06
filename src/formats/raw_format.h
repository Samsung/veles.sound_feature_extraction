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

#include "src/buffer_format.h"
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

class RawFormat : public BufferFormat {
 public:
  RawFormat() noexcept;

  RawFormat(size_t size, int samplingRate)
  throw(InvalidRawFormatParametersException);

  explicit RawFormat(const RawFormat& other) noexcept;

  int SamplingRate() const noexcept;
  size_t Size() const noexcept;

 protected:
  virtual bool EqualsTo(const BufferFormat& other) const noexcept;
  virtual void SetParametersFrom(const BufferFormat& other) noexcept;

 private:
  static const std::string ID_;

  size_t size_;
  int samplingRate_;
};

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_RAW_FORMAT_H_
