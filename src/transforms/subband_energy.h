/*! @file subband_energy.h
 *  @brief Calculate the subband energy.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
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

#ifndef SRC_TRANSFORMS_SUBBAND_ENERGY_H_
#define SRC_TRANSFORMS_SUBBAND_ENERGY_H_

#include "src/transforms/common.h"
#include "src/primitives/wavelet_filter_bank.h"

namespace sound_feature_extraction {
namespace transforms {

/// @brief Calculates the energy of the sub-signals for each subband.
/// @details Calculates
/// \f$\displaystyle \frac{\sum_{j\in S_i}{Value^2(j)}}{N_i}\f$, where
/// \f$S_i\f$ is subband's signal values set and \f$N_i\f$ is the corresponding
/// cardinal number. Thus, the output of this transform is a series of numbers,
/// each being equal to the corresponding subband energy. Numbers count is the
/// same as the length of the binary tree fingerprint.
class SubbandEnergy : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  SubbandEnergy();

  TRANSFORM_INTRO("SubbandEnergy",
                  "Calculates the subband energies (subbands are defined with"
                  "a binary tree fingerprint identical to used in DWPT).",
                  SubbandEnergy)

  TP(tree, TreeFingerprint, kDefaultTreeFingerprint(),
     "The subbands binary tree fingerprint.")

  virtual void Initialize() const override;

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in,
                  float* out) const noexcept override;

 private:
  static TreeFingerprint kDefaultTreeFingerprint() noexcept {
    return TreeFingerprint {
      3, 3, 3,
      4, 4, 4,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      6, 6, 6, 6, 6, 6, 6, 6
    };
  }

  mutable TreeFingerprint offsets_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_SUBBAND_ENERGY_H_
