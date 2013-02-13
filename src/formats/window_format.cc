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

WindowFormat::WindowFormat()
: duration_(0.025f * 16000), samplingRate_(16000) {
}

WindowFormat::WindowFormat(size_t duration, int samplingRate)
: duration_(duration), samplingRate_(samplingRate) {
  assert(duration_ >= MIN_WINDOW_DURATION &&
         duration_ <= MAX_WINDOW_DURATION);
  assert(samplingRate_ >= MIN_SAMPLING_RATE &&
         samplingRate_ <= MAX_SAMPLING_RATE);
}

BufferFormat& WindowFormat::operator=(const BufferFormat& other) {
  if (other.Id() != Id()) {
    throw new InvalidFormatException(Id(), other.Id());
  }
  *this = reinterpret_cast<const WindowFormat&>(other);
  return *this;
}

size_t WindowFormat::Duration() const {
  return duration_;
}

int WindowFormat::SamplingRate() const {
  return samplingRate_;
}

void WindowFormat::SetDuration(size_t value) {
  assert(duration_ >= MIN_WINDOW_DURATION &&
         duration_ <= MAX_WINDOW_DURATION);
  duration_ = value;
}

void WindowFormat::SetSamplingRate(int value) {
  assert(samplingRate_ >= MIN_SAMPLING_RATE &&
         samplingRate_ <= MAX_SAMPLING_RATE);
  samplingRate_ = value;
}

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
