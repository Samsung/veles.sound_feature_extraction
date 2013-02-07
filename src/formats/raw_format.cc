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

const std::string RawFormat::ID_ = "Raw";

RawFormat::RawFormat() noexcept
: BufferFormat(ID_)
, size_(2048 * 2)
, samplingRate_(16000) {
}

RawFormat::RawFormat(size_t size, int samplingRate)
: BufferFormat(ID_)
, size_(size)
, samplingRate_(samplingRate) {
  if (size_ < 64 || size_ > (1 << 30) ||
      samplingRate_ < 2000 || samplingRate_ > 48000) {
    throw new InvalidRawFormatParametersException(size_,
                                                  samplingRate_);
  }
}

RawFormat::RawFormat(const RawFormat& other) noexcept
: BufferFormat(ID_) {
  SetParametersFrom(other);
}

bool RawFormat::EqualsTo(const BufferFormat& other) const noexcept {
  CAST_FORMAT(other, RawFormat, orf);
  return (size_ == orf.size_ && samplingRate_ == orf.samplingRate_);
}

void RawFormat::SetParametersFrom(const BufferFormat& other) noexcept {
  CAST_FORMAT(other, RawFormat, orf);
  size_ = orf.size_;
  samplingRate_ = orf.samplingRate_;
}

int RawFormat::SamplingRate() const noexcept {
  return samplingRate_;
}

size_t RawFormat::Size() const noexcept {
  return size_;
}

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
