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

const float Beat::kInitialBeatsValue = 150.f;
const float Beat::kDifference[4] = { 90.f, 2.f, 0.5f, 0.1f };
const float Beat::kStep[4] = { 1.f, 0.5f, 0.1f, 0.01f };
const int Beat::kMaxFrequency = 4096;
const float Beat::kCoefficient = 120.0f;
const int Beat::kPulses = 3;

Beat::Beat() : buffer_(nullptr, std::free), bands_(1) {
  RegisterSetter("bands", [&](const std::string& value) {
    int iv = Parse<int>("bands", value);
    if (iv < 1) {
      return false;
    }
    bands_ = iv;
    return true;
  });
}

size_t Beat::OnInputFormatChanged(size_t buffersCount) {
  return buffersCount / bands_;
}

void Beat::Initialize() const noexcept {
  buffer_ = std::uniquify(mallocf(this->inputFormat_->Size() * sizeof(float)),
                          std::free);
}

void Beat::Do(const BuffersBase<float*>& in,
              BuffersBase<float>* out) const noexcept {
  for (size_t ini = 0; ini < in.Count(); ini += bands_) {
    auto result = kInitialBeatsValue;
    auto size = this->inputFormat_->Size();
    auto buffer = buffer_.get();
    float step, max_energy = 0;
    float min_beats_per_minute, max_beats_per_minute;
    for (int j = 0;
         j < static_cast<int>(sizeof(kDifference) / sizeof(kDifference[0]));
         j++) {
      min_beats_per_minute = result - kDifference[j];
      if (min_beats_per_minute < 1) {
        min_beats_per_minute = 1;
      }
      max_beats_per_minute = result + kDifference[j];
      step = kStep[j];
      max_energy = 0;
      float beats_per_minute = min_beats_per_minute;
      while (beats_per_minute < max_beats_per_minute) {
        float current_energy = 0;
        int period = floorf(kCoefficient * kMaxFrequency / beats_per_minute)+1;
        auto pulse_offset = period * kPulses;
        for (size_t in_index = ini;
             in_index < ini + bands_ && in_index < in.Count();
             in_index++) {
          memcpy(buffer, in[in_index], pulse_offset * sizeof(float));
          matrix_sub(true, in[in_index] + pulse_offset, in[in_index],
                     size - pulse_offset, 1, buffer + pulse_offset);
          matrix_add(true, buffer + period, buffer, size - period, 1,
                     buffer + period);
          current_energy += calculate_energy(Beat::UseSimd(), buffer, size);
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
