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
#include <simd/convolute.h>
#include "src/primitives/energy.h"
#include <math.h>
#include <string>

namespace SoundFeatureExtraction {
namespace Transforms {

const float Beat::kDifference[4] = { 90.f, 2.f, 0.5f, 0.1f };
const float Beat::kStep[4] = { 1.f, 0.5f, 0.1f, 0.01f };
const int Beat::kMaxFrequency = 4096;
const float Beat::kCoefficient = 120.0f;

void Beat::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<float>* buffers) const noexcept {
  buffers->Initialize(in.Size());
}

void Beat::Do(const BuffersBase<Formats::WindowF>& in,
              BuffersBase<float>* out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    Do(in[i].Data.get(), inputFormat_->Size(), &(*out)[i]);
  }
}

void Beat::Do(const float* input, int length,
              float *output) noexcept {
  float result = 150.f, step, maxEnergy;
  float minBeatPerMinute, maxBeatPerMinute;
  float *X = mallocf(length);
  for (int j = 0; j < 4; ++j) {
    minBeatPerMinute = result - kDifference[j];
    maxBeatPerMinute = result + kDifference[j];
    step = kStep[j];
    maxEnergy = 0;
    float beatPerMinute = minBeatPerMinute;
    while (beatPerMinute < maxBeatPerMinute) {
      float curEnergy = 0;
      int pulses = 3;
      int period = floor(kCoefficient * kMaxFrequency / beatPerMinute);
      convolute_ones(input, length, period, pulses, X);
      curEnergy = calculate_energy(true, X, length);
      if (curEnergy > maxEnergy) {
        maxEnergy = curEnergy;
        result = beatPerMinute;
      }
      beatPerMinute += step;
    }
  }
  free(X);
  *output = result;
}

REGISTER_TRANSFORM(Beat);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
