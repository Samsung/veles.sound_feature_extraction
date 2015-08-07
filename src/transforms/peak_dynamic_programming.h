/*! @file peak_dynamic_programming.h
 *  @brief Accurate average estimation based on dynamic programming.
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

#ifndef SRC_TRANSFORMS_PEAK_DYNAMIC_PROGRAMMING_H_
#define SRC_TRANSFORMS_PEAK_DYNAMIC_PROGRAMMING_H_

#include "src/formats/fixed_array.h"
#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class PeakDynamicProgramming : public TransformBase<
    formats::ArrayFormat<formats::FixedArray<2>>, formats::SingleFormatF> {
 public:
  PeakDynamicProgramming();

  TRANSFORM_INTRO("PeakDynamicProgramming",
                  "Accurate average estimation using dynamic programming "
                  "approach.",
                  PeakDynamicProgramming)

  TP(mind_values, bool, kDefaultMindValues,
     "Whether to favor points with higher values.")

 protected:
  virtual void Do(const BuffersBase<formats::FixedArray<2>*>& in,
                  BuffersBase<float>* out)
      const noexcept override;

  static constexpr bool kDefaultMindValues = false;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_PEAK_DYNAMIC_PROGRAMMING_H_
