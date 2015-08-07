/*! @file short_time_msn.h
 *  @brief New file description.
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

#ifndef SRC_TRANSFORMS_SHORT_TIME_MSN_H_
#define SRC_TRANSFORMS_SHORT_TIME_MSN_H_

#include "src/formats/array_format.h"
#include "src/transform_base.h"

namespace sound_feature_extraction {
namespace transforms {

class ShortTimeMeanScaleNormalization
    : public UniformFormatTransform<formats::ArrayFormatF> {
 public:
  ShortTimeMeanScaleNormalization();

  TRANSFORM_INTRO("STMSN", "Calculate short-time mean and scale normalized "
                           "values, that is,"
                           "$stmsn_n[i] = \frac{w_n[i] - "
                           "\\sum_{k=n-L/2}^{n+L/2}{w_k[i]}}"
                           "{\\max_{k=n-L/2}^{n+L/2}{w_k[i]} -"
                           "\\min_{k=n-L/2}^{n+L/2}{w_k[i]}}$.",
                  ShortTimeMeanScaleNormalization)

  TP(length, int, kDefaultLength, "The amount of local values to average.")

 protected:
  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*>* out) const noexcept override;

  static constexpr int kDefaultLength = 300;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_SHORT_TIME_MSN_H_
