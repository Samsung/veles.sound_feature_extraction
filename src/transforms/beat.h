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
#include <vector>
#include "src/formats/fixed_array.h"
#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class Beat
    : public TransformBase<Formats::ArrayFormatF,
                           Formats::ArrayFormat<Formats::FixedArray<2>>>,
      public TransformLogger<Beat> {
 public:
  Beat();

  TRANSFORM_INTRO("Beat", "Find the tempo of a musical signal.")

  TRANSFORM_PARAMETERS(
      TP("bands", "The number of bands to sum.", "1")
      TP("pulses", "The number of pulses for the convolution.",
         std::to_string(kDefaultPulses))
      TP("min_bpm", "Minimal number of beats per minute.",
         std::to_string(kDefaultMinBPM))
      TP("max_bpm", "Maximal number of beats per minute.",
         std::to_string(kDefaultMaxBPM))
      TP("resolution1", "The bpm resolution during the first pass.",
         std::to_string(kDefaultResolution1))
      TP("resolution2", "The bpm resolution during the second pass.",
         std::to_string(kDefaultResolution2))
      TP("max_peaks", "The number of the most significant peaks to record.",
         std::to_string(kDefaultPeaks))
      TP("debug", "Dump the resulting energy vectors.", "false")
  )

 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override;

  virtual void Initialize() const override;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<Formats::FixedArray<2>*>* out)
      const noexcept override;

  static void CombConvolve(const float* in, size_t size, int pulses,
                           int period, float* out) noexcept;

 private:
  static size_t PulsesLength(int pulses_count, int period) noexcept;
  void CalculateBeatEnergies(const BuffersBase<float*>& in, size_t inIndex,
                             float min_bpm, float max_bpm, float step,
                             std::vector<float>* energies,
                             float* max_energy_bpm_found = nullptr,
                             float* max_energy_found = nullptr) const noexcept;

  static constexpr int kDefaultPulses = 3;
  static constexpr float kDefaultMinBPM = 60.f;
  static constexpr float kDefaultMaxBPM = 240.f;
  static constexpr float kDefaultResolution1 = 2.f;
  static constexpr float kDefaultResolution2 = 0.1f;
  static constexpr int kDefaultPeaks = 3;

  mutable FloatPtr buffer_;
  int bands_;
  int pulses_;
  float min_bpm_;
  float max_bpm_;
  float step1_;
  float step2_;
  int peaks_;
  bool debug_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_BEAT_H_
