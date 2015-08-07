/*! @file beat.h
 *  @brief Beat detection.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>, Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 *
 *  @section License
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */

#ifndef SRC_TRANSFORMS_BEAT_H_
#define SRC_TRANSFORMS_BEAT_H_

#include <mutex>
#include <tuple>
#include <vector>
#include "src/formats/fixed_array.h"
#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class Beat
    : public OmpAwareTransform<formats::ArrayFormatF,
                               formats::ArrayFormat<formats::FixedArray<2>>>,
      public TransformLogger<Beat> {
 public:
  Beat();

  TRANSFORM_INTRO("Beat", "Find the tempo of a musical signal.", Beat)

  TP(bands, int, kDefaultBands, "The number of bands to sum.")
  TP(pulses, int, kDefaultPulses, "The number of pulses for the convolution.")
  TP(min_bpm, float, kDefaultMinBPM, "Minimal number of beats per minute.")
  TP(max_bpm, float, kDefaultMaxBPM, "Maximal number of beats per minute.")
  TP(resolution1, float, kDefaultResolution1,
     "The bpm resolution during the first pass.")
  TP(resolution2, float, kDefaultResolution2,
     "The bpm resolution during the second pass.")
  TP(peaks, int, kDefaultPeaks,
     "The number of the most significant peaks to record.")
  TP(debug, bool, kDefaultDebug, "Dump the resulting energy vectors.")

  virtual bool BufferInvariant() const noexcept override final {
    return false;
  }

  virtual void Initialize() const override;

 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<formats::FixedArray<2>*>* out)
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

  static constexpr int kDefaultBands = 1;
  static constexpr int kDefaultPulses = 3;
  static constexpr float kDefaultMinBPM = 60.f;
  static constexpr float kDefaultMaxBPM = 240.f;
  static constexpr float kDefaultResolution1 = 2.f;
  static constexpr float kDefaultResolution2 = 0.1f;
  static constexpr int kDefaultPeaks = 3;
  static constexpr bool kDefaultDebug = false;

  mutable std::vector<ThreadSafeFloatPtr> buffers_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_BEAT_H_
