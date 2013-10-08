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

#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class Beat
    : public TransformBase<Formats::RawFormatF, Formats::SingleFormatF> {
 public:
  Beat();

  TRANSFORM_INTRO("Beat", "Find the tempo of a musical signal.")

  TRANSFORM_PARAMETERS(
      TP("bands", "The number of bands to sum.", "1")
      TP("pulses", "The number of pulses in the convolution.",
         std::to_string(kDefaultPulses))
  )

 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override;

  virtual void Initialize() const noexcept override;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float>* out) const noexcept override;

  static void CombConvolve(const float* in, size_t size, int pulses,
                           int period, float* out) noexcept;

 private:
  static constexpr float MinBeatsPerMinuteValue();
  static size_t PulsesLength(int pulses_count, int period) noexcept;

  static constexpr float kInitialBeatsValue = 150.f;
  static constexpr size_t kStepsCount = 4;
  static constexpr float kDifference[kStepsCount] { 90.f, 2.f, 0.5f, 0.1f };
  static constexpr float kStep[kStepsCount] { 1.f, 0.5f, 0.1f, 0.01f };
  static constexpr int kDefaultPulses = 3;
  mutable std::unique_ptr<float, decltype(&std::free)> buffer_;
  int bands_;
  int pulses_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_BEAT_H_
