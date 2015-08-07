/*! @file rolloff.h
 *  @brief Measure of spectral shape.
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

#ifndef SRC_TRANSFORMS_ROLLOFF_H_
#define SRC_TRANSFORMS_ROLLOFF_H_

#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

/// @brief \f$R: \displaystyle\sum_{f=0}^{f=R}{Value[f]} =
/// ratio \times \displaystyle\sum_{f=0}^{f=N}{Value[f]}\f$.
class Rolloff : public OmpTransformBase<formats::ArrayFormatF,
                                        formats::SingleFormatF>  {
 public:
  Rolloff();

  TRANSFORM_INTRO("Rolloff", "Measure of spectral shape.", Rolloff)

  TP(ratio, float, kDefaultRatio,
     "The ratio between the partial sum and the whole sum.")

 protected:
  virtual void Do(const float* in, float* out) const noexcept override;

  static int Do(bool simd, const float* input, size_t length,
                float ratio) noexcept;

 private:
  static constexpr float kDefaultRatio = 0.85f;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_ROLLOFF_H_
