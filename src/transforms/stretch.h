/*! @file stretch.h
 *  @brief Stretch the signal by duplicating its values.
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

#ifndef SRC_TRANSFORMS_STRETCH_H_
#define SRC_TRANSFORMS_STRETCH_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class Stretch : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  Stretch();

  TRANSFORM_INTRO("Stretch", "Stretch the signal by an integer factor.",
                  Stretch)

  TP(factor, float, kDefaultFactor,
     "Stretch the signal by duplicating its values.")
  TP(center, bool, kDefaultCenter,
     "Center the intervals, extending the corners (useful for stretching after "
     "splitting into windows.).")

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in, float* out) const noexcept override;

  static constexpr float kDefaultFactor = 1;
  static constexpr bool kDefaultCenter = false;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_STRETCH_H_
