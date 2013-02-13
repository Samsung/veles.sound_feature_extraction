/*! @file raw_format.cc
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

#include "src/formats/raw_format.h"

namespace SpeechFeatureExtraction {
namespace Formats {

RawFormat::RawFormat() noexcept
: size_(2048 * 2)
, samplingRate_(16000) {
}

BufferFormat& RawFormat::operator=(const BufferFormat& other) {
  if (other.Id() != Id()) {
    throw new InvalidFormatException(Id(), other.Id());
  }
  *this = reinterpret_cast<const RawFormat&>(other);
  return *this;
}

RawFormat::RawFormat(size_t size, int samplingRate)
: size_(size)
, samplingRate_(samplingRate) {
  if (size_ < 64 || size_ > (1 << 30) ||
      samplingRate_ < 2000 || samplingRate_ > 48000) {
    throw new InvalidRawFormatParametersException(size_,
                                                  samplingRate_);
  }
}

int RawFormat::SamplingRate() const noexcept {
  return samplingRate_;
}

size_t RawFormat::Size() const noexcept {
  return size_;
}

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
