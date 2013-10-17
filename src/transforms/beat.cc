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

Beat::Beat()
    : buffer_(nullptr, std::free),
      bands_(1),
      pulses_(kDefaultPulses),
      debug_(false) {
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
  RegisterSetter("debug", [&](const std::string& value) {
    debug_ = Parse<bool>("debug", value);
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
              BuffersBase<Formats::FixedArray<2>>* out) const noexcept {
  for (size_t ini = 0; ini < in.Count(); ini += bands_) {
    auto result = kInitialBeatsValue;
    float result_energy;
    auto size = this->inputFormat_->Size();
    auto buffer = buffer_.get();
    for (int j = 0; j < static_cast<int>(kStepsCount); j++) {
      const float min_bpm = result - kDifference[j];
      const float max_bpm = result + kDifference[j];
      const float step = kStep[j];
      result_energy = 0;
      int search_size = floorf((max_bpm - min_bpm) / step);
      float energies[search_size];
      for (int i = 0; i < search_size; i++) {
        float bpm = min_bpm + step * i;
        // 60 is the number of seconds in one minute
        int period = floorf(60 * inputFormat_->SamplingRate() / bpm);
        float current_energy = 0;
        size_t conv_length = size + PulsesLength(pulses_, period) - 1;
        for (size_t i = ini; i < ini + bands_ && i < in.Count(); i++) {
          CombConvolve(in[i], size, pulses_, period, buffer);
          current_energy += calculate_energy(Beat::UseSimd(), buffer,
                                             conv_length) * conv_length;
        }
        energies[i] = current_energy;
        if (current_energy > result_energy) {
          result_energy = current_energy;
          result = bpm;
        }
      }

      if (j == 0) {
        // Fix underestimation and overestimation errors
        std::tie(result, result_energy) = FixBorderErrors(
            energies, min_bpm,max_bpm, step, result, result_energy);
      }

      if (debug_) {
        std::string dump("----");
        dump += std::to_string(j + 1) + "---- ";
        dump += std::to_string(result) + '\n';
        for (int i = 0; i < search_size; i++) {
          dump += std::to_string(energies[i]) + "    ";
          if (i % 10 == 0 && i > 0) {
            dump += '\n';
          }
        }
        INF("%s\n----\n", dump.c_str());
      }
    }
    (*out)[ini / bands_][0] = result;
    (*out)[ini / bands_][1] = result_energy;
  }
}

std::tuple<float, float> Beat::FixBorderErrors(
    const float* energies, float min_bpm, float max_bpm, float step,
    float result, float result_energy) noexcept {
  int search_size = floorf((max_bpm - min_bpm) / step);
  int min_boundary = 1, max_boundary = 1;
  auto calc_underest = [&]() {
    return result < min_bpm + (min_boundary + 0.5) * step;
  };
  auto calc_overest = [&]() {
    return result > max_bpm - (max_boundary + 0.5) * step;
  };
  bool underest = calc_underest();
  bool overest = calc_overest();
  while (min_boundary != search_size - max_boundary && (underest || overest)) {
    if (underest) {
      min_boundary++;
    } else {
      max_boundary++;
    }
    result_energy = 0;
    for (int i = min_boundary; i <= search_size - max_boundary; i++) {
      float current_energy = energies[i];
      if (current_energy > result_energy) {
        result_energy = current_energy;
        result = min_bpm + step * i;
      }
    }
    underest = calc_underest();
    overest = calc_overest();
  }
  return std::make_tuple(result, result_energy);
}

REGISTER_TRANSFORM(Beat);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
