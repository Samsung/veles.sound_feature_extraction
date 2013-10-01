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
#include <simd/matrix.h>
#include "src/make_unique.h"
#include "src/primitives/energy.h"
#include <cmath>

namespace SoundFeatureExtraction {
namespace Transforms {

const float Beat:: kDifference[4] = { 90.f, 2.f, 0.5f, 0.1f };
const float Beat:: kStep[4] = { 1.f, 0.5f, 0.1f, 0.01f };
const int Beat:: kMaxFrequency = 4096;
const float Beat:: kCoefficient = 120.0f;

Beat::Beat() : buffer_(nullptr, std::free) {
}

void Beat::Initialize() const noexcept {
  buffer_ = std::uniquify(mallocf(this->inputFormat_->Size() * sizeof(float)),
                          std::free);
}

void Beat::Do(const float* in, float* out) const noexcept {
  auto result = 150.f;
  auto size = this->inputFormat_->Size();
  auto buffer = buffer_.get();
  float step, maxEnergy = 0;
  float minBeatsPerMinute, maxBeatsPerMinute;
  for (int j = 0; j < 4; ++j) {
    minBeatsPerMinute = result - kDifference[j];
    if (minBeatsPerMinute < 1) {
      minBeatsPerMinute = 1;
    }
    maxBeatsPerMinute = result + kDifference[j];
    step = kStep[j];
    maxEnergy = 0;
    float beatsPerMinute = minBeatsPerMinute;
    while (beatsPerMinute < maxBeatsPerMinute) {
      float curEnergy = 0;
      int pulses = 3;
      int period = floorf(kCoefficient * kMaxFrequency / beatsPerMinute) + 1;
      auto pulse_offset = period * pulses;
      memcpy(buffer, in, pulse_offset * sizeof(float));
      matrix_sub(true, in + pulse_offset, in, size - pulse_offset, 1,
                 buffer + pulse_offset);
      matrix_add(true, buffer + period, buffer, size - period, 1,
                 buffer + period);
      curEnergy = calculate_energy(Beat::UseSimd(), buffer, size);
      if (curEnergy > maxEnergy) {
        maxEnergy = curEnergy;
        result = beatsPerMinute;
      }
      beatsPerMinute += step;
    }
  }
  *out = result;
}

REGISTER_TRANSFORM(Beat);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
