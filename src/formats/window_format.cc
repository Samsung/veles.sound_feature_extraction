/*! @file window_format.cc
 *  @brief Raw data frame format.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/formats/window_format.h"
#include <assert.h>
#include "src/formats/format_limits.h"

namespace SpeechFeatureExtraction {
namespace Formats {

WindowFormat::WindowFormat() noexcept
: duration_(DEFAULT_WINDOW_DURATION)
, samplingRate_(DEFAULT_SAMPLING_RATE) {
}

WindowFormat::WindowFormat(const WindowFormat& other) noexcept
: duration_(other.duration_)
, samplingRate_(samplingRate_) {
}

WindowFormat::WindowFormat(size_t duration, int samplingRate)
: duration_(duration), samplingRate_(samplingRate) {
  ValidateDuration(duration_);
  ValidateSamplingRate(samplingRate_);
}

BufferFormat& WindowFormat::operator=(const BufferFormat& other) {
  if (other.Id() != Id()) {
    throw new InvalidFormatException(Id(), other.Id());
  }
  *this = reinterpret_cast<const WindowFormat&>(other);
  return *this;
}

size_t WindowFormat::Duration() const noexcept {
  return duration_;
}

int WindowFormat::SamplingRate() const noexcept {
  return samplingRate_;
}

void WindowFormat::SetDuration(size_t value) {
  ValidateDuration(value);
  duration_ = value;
}

void WindowFormat::SetSamplingRate(int value) {
  ValidateSamplingRate(value);
  samplingRate_ = value;
}

void WindowFormat::ValidateDuration(size_t value) {
  if (value < MIN_WINDOW_DURATION || value > MAX_WINDOW_DURATION) {
    throw new InvalidWindowFormatDurationException(value);
  }
}

void WindowFormat::ValidateSamplingRate(int value) {
  if (value < MIN_SAMPLING_RATE || value > MAX_SAMPLING_RATE) {
    throw new InvalidWindowFormatSamplingRateException(value);
  }
}

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
