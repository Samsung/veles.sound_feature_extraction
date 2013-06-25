/*! @file beat.h
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

#ifndef SRC_TRANSFORMS_BEAT_H_
#define SRC_TRANSFORMS_BEAT_H_

#include <string>
#include "src/formats/window_format.h"
#include "src/formats/single_format.h"
#include "src/formats/raw_format.h"
#include "src/transform_base.h"
#include "src/primitives/energy.h"
#include <math.h>
#include <string>

namespace SoundFeatureExtraction {
namespace Transforms {

template <class F>
class Beat : public TransformBase<F, Formats::SingleFormatF> {
 public:
  TRANSFORM_INTRO("Beat", "Find the tempo of a musical signal.")

  TRANSFORM_PARAMETERS()

 protected:
  virtual void InitializeBuffers(
      const BuffersBase<typename F::BufferType>& in,
      BuffersBase<float>* buffers) const noexcept {
    buffers->Initialize(in.Size());
  }

  virtual void Do(const BuffersBase<typename F::BufferType>& in,
                  BuffersBase<float>* out) const noexcept {
    for (size_t i = 0; i < in.Size(); ++i) {
      Do(in[i].Data.get(), this->inputFormat_->Size() ,(*out)[i]);
    }
  }

  static void Do(const typename F::BufferElementType* input,
                 int length, float& output) noexcept {
    auto result = 150.f;
    float step, maxEnergy = 0;
    float minBeatPerMinute, maxBeatPerMinute;
    float *X = mallocf(length * sizeof(float));
    for (int j = 0; j < 4; ++j) {
      minBeatPerMinute = result - kDifference[j];
      maxBeatPerMinute = result + kDifference[j];
      step = kStep[j];
      maxEnergy = 0;
      float beatPerMinute = minBeatPerMinute;
      while (beatPerMinute < maxBeatPerMinute) {
        float curEnergy = 0;
        int pulses = 3;
        int period = floorf(kCoefficient * kMaxFrequency / beatPerMinute) + 1;
        for (int i = 0; i < (int)length; ++i) {
          X[i] = input[i] + (i >= period ? X[i - period] : 0) -
              (i >= period * pulses ? input[i - period * pulses] : 0);
        }
        curEnergy = calculate_energy(Beat::UseSimd(), X, length);
        if (curEnergy > maxEnergy) {
          maxEnergy = curEnergy;
          result = beatPerMinute;
        }
        beatPerMinute += step;
      }
    }
    free(X);
    output = result;
  }

 private:
  static const float kDifference[4];
  static const float kStep[4];
  static const int kMaxFrequency;
  static const float kCoefficient;
};

template <class F>
const float Beat<F> :: kDifference[4] = { 90.f, 2.f, 0.5f, 0.1f };

template <class F>
const float Beat<F> :: kStep[4] = { 1.f, 0.5f, 0.1f, 0.01f };

template <class F>
const int Beat<F> :: kMaxFrequency = 4096;

template <class F>
const float Beat<F> :: kCoefficient = 120.0f;

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_BEAT_H_
