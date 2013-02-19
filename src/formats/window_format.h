/*! @file window_format.h
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

#ifndef RAW_WINDOW_FORMAT_H_
#define RAW_WINDOW_FORMAT_H_

#include "src/buffers_base.h"
#include "src/formats/format_limits.h"

namespace SpeechFeatureExtraction {
namespace Formats {

class InvalidWindowFormatDurationException : public ExceptionBase {
 public:
  InvalidWindowFormatDurationException(size_t duration)
  : ExceptionBase("Duration " + std::to_string(duration) +
                  " is not supported or invalid.") {}
};

class InvalidWindowFormatSamplingRateException : public ExceptionBase {
 public:
  InvalidWindowFormatSamplingRateException(int samplingRate)
  : ExceptionBase("Sampling rate " + std::to_string(samplingRate) +
                  " is not supported or invalid.") {}
};

template <class T>
struct Window {
  T* Chunk;
};

typedef Window<int16_t> Window16;
typedef Window<int32_t> Window32;
typedef Window<float> WindowF;

template <class T>
class WindowFormat : public BufferFormatBase<Window<T>> {
 public:
  WindowFormat() noexcept
  : duration_(DEFAULT_WINDOW_DURATION)
  , samplingRate_(DEFAULT_SAMPLING_RATE) {
  }

  WindowFormat(const WindowFormat& other) noexcept
  : duration_(other.duration_)
  , samplingRate_(samplingRate_) {
  }

  WindowFormat(size_t duration, int samplingRate)
  : duration_(duration), samplingRate_(samplingRate) {
    ValidateDuration(duration_);
    ValidateSamplingRate(samplingRate_);
  }

  BufferFormat& operator=(const BufferFormat& other) {
    if (other.Id() != WindowFormat<T>::Id()) {
      throw new InvalidFormatException(WindowFormat<T>::Id(), other.Id());
    }
    *this = reinterpret_cast<const WindowFormat&>(other);
    return *this;
  }

  size_t Duration() const noexcept {
    return duration_;
  }

  void SetDuration(size_t value) {
    ValidateDuration(value);
    duration_ = value;
  }

  int SamplingRate() const noexcept {
    return samplingRate_;
  }

  void SetSamplingRate(int value) {
    ValidateSamplingRate(value);
    samplingRate_ = value;
  }

  size_t SamplesCount() const noexcept {
    return duration_ * samplingRate_ / 1000;
  }

 private:
  size_t duration_;
  int samplingRate_;

  static void ValidateDuration(size_t value) {
    if (value < MIN_WINDOW_DURATION || value > MAX_WINDOW_DURATION) {
      throw new InvalidWindowFormatDurationException(value);
    }
  }

  static void ValidateSamplingRate(int value) {
    if (value < MIN_SAMPLING_RATE || value > MAX_SAMPLING_RATE) {
      throw new InvalidWindowFormatSamplingRateException(value);
    }
  }
};

typedef WindowFormat<int16_t> WindowFormat16;
typedef WindowFormat<int32_t> WindowFormat32;
typedef WindowFormat<float> WindowFormatF;

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_RAW_WINDOW_FORMAT_H_
