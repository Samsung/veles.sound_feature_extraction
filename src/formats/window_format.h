/*! @file window_format.h
 *  @brief Window frame format.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FORMATS_WINDOW_FORMAT_H_
#define SRC_FORMATS_WINDOW_FORMAT_H_

#include <assert.h>
#include "src/buffers_base.h"
#include "src/formats/format_limits.h"
#include "src/primitives/memory.h"

namespace SpeechFeatureExtraction {
namespace Formats {

class InvalidWindowFormatDurationException : public ExceptionBase {
 public:
  explicit InvalidWindowFormatDurationException(size_t duration)
  : ExceptionBase("Duration " + std::to_string(duration) +
                  " is not supported or invalid.") {}
};

class InvalidWindowFormatSamplingRateException : public ExceptionBase {
 public:
  explicit InvalidWindowFormatSamplingRateException(int samplingRate)
  : ExceptionBase("Sampling rate " + std::to_string(samplingRate) +
                  " is not supported or invalid.") {}
};

template <typename T>
struct Window {
  std::shared_ptr<T> Data;

  explicit Window(size_t size)
  : Data(reinterpret_cast<T*>(
      malloc_aligned(size * sizeof(T))),
      [](T *ptr) { free(ptr); }) {
  }

  Window(const Window& other) : Data(other.Data) {
  }

  explicit Window(T* ptr) : Data(ptr, [](T* p) {}) {
  }

  Window& operator=(const Window& other) {
    Data = other.Data;
    return *this;
  }
};

typedef Window<int16_t> Window16;
typedef Window<int32_t> Window32;
typedef Window<float> WindowF;

template <class T>
class WindowFormat : public BufferFormatBase<Window<T>> {
 public:
  WindowFormat() noexcept
  : duration_(DEFAULT_WINDOW_DURATION)
  , samplingRate_(DEFAULT_SAMPLING_RATE)
  , size_(SamplesCount()) {
  }

  WindowFormat(const WindowFormat& other) noexcept
  : duration_(other.duration_)
  , samplingRate_(other.samplingRate_)
  , size_(SamplesCount()) {
  }

  WindowFormat(size_t duration, int samplingRate)
  : duration_(duration)
  , samplingRate_(samplingRate)
  , size_(SamplesCount()) {
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

  size_t Size() const noexcept {
    return size_;
  }

  void SetSize(size_t value) noexcept {
    // We speculatively allocate only 2 extra numbers
    assert(value <= SamplesCount() + 2);
    size_ = value;
  }

 private:
  size_t duration_;
  int samplingRate_;
  size_t size_;

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
#endif  // SRC_FORMATS_WINDOW_FORMAT_H_
