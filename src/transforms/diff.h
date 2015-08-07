/*! @file diff.h
 *  @brief Find the difference from one sample to the next.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
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

#ifndef SRC_TRANSFORMS_DIFF_H_
#define SRC_TRANSFORMS_DIFF_H_

#include "src/transforms/common.h"
#include <vector>

namespace sound_feature_extraction {
namespace transforms {

class Diff : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  Diff();

  TRANSFORM_INTRO("Diff", "Differentiate the signal.", Diff)

  TP(rectify, bool, false,
     "Perform half-wave rectification (retain only if the difference is "
     "positive).")
  TP(swt, int, kNoSWT,
     "Smoothly differentiate using Stationary Wavelet Transform "
     "(db1) of the specified level. The level must be greater than "
     "or equal to 0. If set to zero, this parameter is ignored.")

 protected:
  virtual void Initialize() const override;

  virtual void Do(const float* in, float* out) const noexcept override;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;

  static void DoRectify(bool simd, const float* input, int length,
                        float* output) noexcept;

  static void Rectify(bool simd, const float* input, int length,
                      float* output) noexcept;

  static constexpr int kNoSWT = 0;

 private:
  mutable FloatPtr swt_buffer_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_DIFF_H_
