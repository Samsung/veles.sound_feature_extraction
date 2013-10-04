/*! @file buffer_format.h
 *  @brief Buffer format interface class.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_BUFFER_FORMAT_H_
#define SRC_BUFFER_FORMAT_H_

#include <functional>
#include <set>
#include "src/exceptions.h"

namespace SoundFeatureExtraction {

namespace Formats {

class InvalidSamplingRateException : public ExceptionBase {
 public:
  explicit InvalidSamplingRateException(int samplingRate)
  : ExceptionBase("Sampling rate " + std::to_string(samplingRate) +
                  " is not supported or invalid.") {}
};

}  // namespace Formats

class InvalidFormatException : public ExceptionBase {
 public:
  InvalidFormatException(const std::string& idMine,
                         const std::string& idYours)
  : ExceptionBase("Attempted to cast to format \"" + idMine +
                  "\" from format \"" + idYours + "\".") {}
};

class Buffers;

class BufferFormat {
 public:
  explicit BufferFormat(const std::string& id);
  BufferFormat(const std::string& id, int samplingRate);
  virtual ~BufferFormat() = default;

  bool operator==(const BufferFormat& other) const noexcept;
  bool operator!=(const BufferFormat& other) const noexcept;

  size_t SizeInBytes() const noexcept;
  virtual size_t UnalignedSizeInBytes() const noexcept = 0;

  int SamplingRate() const noexcept;
  void SetSamplingRate(int value);
  void CopySourceDetailsFrom(const BufferFormat& format) noexcept;

  virtual const std::string& Id() const noexcept;

  virtual void Validate(const Buffers& buffers) const = 0;

  virtual std::string Dump(const Buffers& buffers) const = 0;

  static void ValidateSamplingRate(int value);

  template <typename T>
  static T Aligned(T value) noexcept {
    return (value & (0x80 - 1)) == 0? value : (value & ~(0x80 - 1)) + 0x80;
  }

 private:
  static const int MIN_SAMPLING_RATE = 2000;
  static const int MAX_SAMPLING_RATE = 48000;

  std::string id_;
  int samplingRate_;
};

}  // namespace SoundFeatureExtraction
#endif  // SRC_BUFFER_FORMAT_H_
