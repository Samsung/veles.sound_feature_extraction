/*! @file beat.cc
 *  @brief Beat detection.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/beat.h"
#include "src/make_unique.h"
#include "src/primitives/energy.h"
#include <cmath>

namespace SoundFeatureExtraction {
namespace Transforms {

constexpr float Beat::kDifference[Beat::kStepsCount];
constexpr float Beat::kStep[Beat::kStepsCount];

Beat::Beat() : buffer_(nullptr, std::free), bands_(1), pulses_(kDefaultPulses) {
  RegisterSetter("bands", [&](const std::string& value) {
    int iv = Parse<int>("bands", value);
    if (iv < 1) {
      return false;
    }
    bands_ = iv;
    return true;
  });
  RegisterSetter("pulses", [&](const std::string& value) {
    int iv = Parse<int>("pulses", value);
    if (iv < 1) {
      return false;
    }
    pulses_ = iv;
    return true;
  });
}

constexpr float Beat::MinBeatsPerMinuteValue() {
  return kInitialBeatsValue - kDifference[0] - kDifference[1] -
      kDifference[2] - kDifference[3];
}

size_t Beat::PulsesLength(int pulses_count, int period) noexcept {
  return (pulses_count - 1) * period + 1;
}

size_t Beat::OnInputFormatChanged(size_t buffersCount) {
  return buffersCount / bands_;
}

void Beat::Initialize() const noexcept {
  float max_period = floorf(60 * inputFormat_->SamplingRate() /
                            MinBeatsPerMinuteValue());
  size_t max_pulses_length = PulsesLength(pulses_, max_period);
  buffer_ = std::uniquify(mallocf(
      (this->inputFormat_->Size() + max_pulses_length - 1) * sizeof(float)),
      std::free);
}

void Beat::CombConvolve(const float* in, size_t size, int pulses,
                        int period, float* out) noexcept {
  int pp = PulsesLength(pulses, period);
  for (int i = 0; i < pp - 1; i++) {
    float res = 0;
    for (int j = 0; j <= i; j += period) {
      res += in[i - j];
    }
    out[i] = res;
  }

  for (int i = pp - 1; i < static_cast<int>(size); i++) {
    float res = 0;
    for (int j = 0; j < pp; j += period) {
      res += in[i - j];
    }
    out[i] = res;
  }

  for (int i = size; i < static_cast<int>(size + pp - 1); i++) {
    float res = 0;
    for (int j = ((i - size) / period + 1) * period; j < pp; j += period) {
      res += in[i - j];
    }
    out[i] = res;
  }
}

void Beat::Do(const BuffersBase<float*>& in,
              BuffersBase<float>* out) const noexcept {
  for (size_t ini = 0; ini < in.Count(); ini += bands_) {
    auto result = kInitialBeatsValue;
    auto size = this->inputFormat_->Size();
    auto buffer = buffer_.get();
    float step, max_energy = 0;
    float min_beats_per_minute, max_beats_per_minute;
    for (int j = 0; j < static_cast<int>(kStepsCount); j++) {
      min_beats_per_minute = result - kDifference[j];
      if (min_beats_per_minute < 1) {
        min_beats_per_minute = 1;
      }
      max_beats_per_minute = result + kDifference[j];
      step = kStep[j];
      max_energy = 0;
      float beats_per_minute = min_beats_per_minute;
      while (beats_per_minute < max_beats_per_minute) {
        // 60 is the number of seconds in one minute
        int period = floorf(60 * inputFormat_->SamplingRate() /
                            beats_per_minute);
        float current_energy = 0;
        size_t conv_length = size + PulsesLength(pulses_, period) - 1;
        for (size_t i = ini; i < ini + bands_ && i < in.Count(); i++) {
          CombConvolve(in[i], size, pulses_, period, buffer);
          current_energy += calculate_energy(Beat::UseSimd(), buffer,
                                             conv_length) * conv_length;
        }
        if (current_energy > max_energy) {
          max_energy = current_energy;
          result = beats_per_minute;
        }
        beats_per_minute += step;
      }
    }
    (*out)[ini / bands_] = result;
  }
}

REGISTER_TRANSFORM(Beat);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
