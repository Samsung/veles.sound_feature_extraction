/*! @file preemphasis.h
 *  @brief Number of time domain zero crossings of the signal.
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

#ifndef SRC_TRANSFORMS_PREEMPHASIS_H_
#define SRC_TRANSFORMS_PREEMPHASIS_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class Preemphasis : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  Preemphasis();

  TRANSFORM_INTRO("Preemphasis", "Filter the signal with a first-order "
                                 "high-pass filter y[n] = 1 - k * x[n - 1].",
                  Preemphasis)

  TP(value, float, kDefaultValue,
     "The filter coefficient from range (0..1]. "
     "The higher, the more emphasis occurs.")

 protected:
  static constexpr float kDefaultValue = 0.9f;

  virtual void Do(const float* in, float* out) const noexcept override;

  static void Do(bool simd, const float* input, size_t length,
                 float k, float* output) noexcept;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_PREEMPHASIS_H_
