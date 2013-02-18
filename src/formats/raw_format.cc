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
#include "src/formats/format_limits.h"

namespace SpeechFeatureExtraction {
namespace Formats {

RawFormat::RawFormat() noexcept
: size_(DEFAULT_SIZE)
, samplingRate_(DEFAULT_SAMPLING_RATE) {
}

BufferFormat& RawFormat::operator=(const BufferFormat& other) {
  if (other.Id() != Id()) {
    throw new InvalidFormatException(Id(), other.Id());
  }
  *this = reinterpret_cast<const RawFormat&>(other);
  return *this;
}

RawFormat::RawFormat(const RawFormat& other) noexcept
: size_(other.size_)
, samplingRate_(other.samplingRate_) {
}

RawFormat::RawFormat(size_t size, int samplingRate)
: size_(size)
, samplingRate_(samplingRate) {
  ValidateSize(size_);
  ValidateSamplingRate(samplingRate_);
}

int RawFormat::SamplingRate() const noexcept {
  return samplingRate_;
}

size_t RawFormat::Size() const noexcept {
  return size_;
}

void RawFormat::SetSamplingRate(int value) {
  ValidateSamplingRate(value);
  samplingRate_ = value;
}

void RawFormat::SetSize(size_t value) {
  ValidateSize(value);
  size_ = value;
}

void RawFormat::ValidateSize(size_t value) {
  if (value < MIN_RAW_SIZE || value > MAX_RAW_SIZE) {
    throw new InvalidRawFormatSizeException(value);
  }
}

void RawFormat::ValidateSamplingRate(int value) {
  if (value < MIN_SAMPLING_RATE || value > MAX_SAMPLING_RATE) {
    throw new InvalidRawFormatSamplingRateException(value);
  }
}

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
