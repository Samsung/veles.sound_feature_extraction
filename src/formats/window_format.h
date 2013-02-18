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

struct Window {
  const int16_t* Chunk;
};

class WindowFormat : public BufferFormatBase<Window> {
 public:
  WindowFormat() noexcept;
  WindowFormat(const WindowFormat& other) noexcept;
  WindowFormat(size_t duration, int samplingRate);

  BufferFormat& operator=(const BufferFormat& other);

  size_t Duration() const noexcept;
  void SetDuration(size_t value);

  int SamplingRate() const noexcept;
  void SetSamplingRate(int value);

 private:
  size_t duration_;
  int samplingRate_;

  static void ValidateDuration(size_t value);
  static void ValidateSamplingRate(int value);
};

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_RAW_WINDOW_FORMAT_H_
