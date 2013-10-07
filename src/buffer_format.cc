/*! @file buffer_format.cc
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

#include "src/buffer_format.h"
#include <assert.h>

namespace SoundFeatureExtraction {

BufferFormat::BufferFormat(const std::string& id, int samplingRate)
    : id_(id),
      samplingRate_(samplingRate) {
  ValidateSamplingRate(samplingRate_);
}

BufferFormat::BufferFormat(const std::string& id)
    : id_(id),
      samplingRate_(0) {
}

const std::string& BufferFormat::Id() const noexcept {
  return id_;
}

bool BufferFormat::operator==(const BufferFormat& other) const noexcept {
  return this->id_ == other.id_;
}

bool BufferFormat::operator!=(const BufferFormat& other) const noexcept {
  return this->id_ != other.id_;
}

void BufferFormat::CopySourceDetailsFrom(const BufferFormat& format) noexcept {
  SetSamplingRate(format.SamplingRate());
}

size_t BufferFormat::SizeInBytes() const noexcept {
  return Aligned(UnalignedSizeInBytes());
}

 int BufferFormat::SamplingRate() const noexcept {
  assert(samplingRate_ > 0);
  return samplingRate_;
}

void BufferFormat::SetSamplingRate(int value) {
  ValidateSamplingRate(value);
  samplingRate_ = value;
}

void BufferFormat::ValidateSamplingRate(int value) {
  if (value < kMinSamplingRate || value > kMaxSamplingRate) {
    throw Formats::InvalidSamplingRateException(value);
  }
}

}  // namespace SoundFeatureExtraction
