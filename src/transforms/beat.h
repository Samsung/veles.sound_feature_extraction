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

#include <tuple>
#include "src/formats/fixed_array.h"
#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class Beat
    : public TransformBase<Formats::RawFormatF,
                           Formats::SingleFormat<Formats::FixedArray<2>>>,
      public TransformLogger<Beat> {
 public:
  Beat();

  TRANSFORM_INTRO("Beat", "Find the tempo of a musical signal.")

  TRANSFORM_PARAMETERS(
      TP("bands", "The number of bands to sum.", "1")
      TP("pulses", "The number of pulses in the convolution.",
         std::to_string(kDefaultPulses))
      TP("debug", "Dump the resulting energy vectors.", "false")
  )

 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override;

  virtual void Initialize() const noexcept override;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<Formats::FixedArray<2>>* out)
      const noexcept override;

  static void CombConvolve(const float* in, size_t size, int pulses,
                           int period, float* out) noexcept;

  static std::tuple<float, float> FixBorderErrors(
      const float* energies, float min_bpm, float max_bpm, float step,
      float result, float result_energy) noexcept;

 private:
  static constexpr float MinBeatsPerMinuteValue();
  static size_t PulsesLength(int pulses_count, int period) noexcept;

  static constexpr float kInitialBeatsValue = 150.f;
  static constexpr size_t kStepsCount = 3;
  static constexpr float kDifference[kStepsCount] { 90.f, 2.f, 0.5f };
  static constexpr float kStep[kStepsCount] { 2.0f, 0.5f, 0.05f };
  static constexpr int kDefaultPulses = 3;
  mutable FloatPtr buffer_;
  int bands_;
  int pulses_;
  bool debug_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_BEAT_H_
