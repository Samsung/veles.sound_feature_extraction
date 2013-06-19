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
  explicit BufferFormat(const std::string& id) noexcept;
  BufferFormat(const BufferFormat& other) noexcept;
  BufferFormat& operator=(const BufferFormat& other) noexcept;

  virtual ~BufferFormat() noexcept {}

  virtual const std::string& Id() const noexcept;

  bool operator==(const BufferFormat& other) const noexcept;
  bool operator!=(const BufferFormat& other) const noexcept;

  virtual std::function<void(void*)> Destructor() const noexcept = 0;  // NOLINT(*)

  virtual bool MustReallocate(const BufferFormat& other) const noexcept = 0;

  virtual size_t PayloadSizeInBytes() const noexcept = 0;

  virtual const void* PayloadPointer(const void* buffer) const noexcept = 0;

  virtual int SamplingRate() const noexcept = 0;

  virtual void SetSamplingRate(int value) = 0;

  virtual void Validate(const Buffers& buffers) const = 0;

  virtual std::string Dump(const Buffers& buffers) const = 0;

  virtual void DeriveFrom(const BufferFormat& format) noexcept;

  static const int MIN_SAMPLING_RATE = 2000;
  static const int MAX_SAMPLING_RATE = 48000;

 private:
  std::string id_;
};

#define CAST_FORMAT(other, sibling, var) \
    const sibling& var = reinterpret_cast<const sibling&>(other)

}  // namespace SoundFeatureExtraction
#endif  // SRC_BUFFER_FORMAT_H_
