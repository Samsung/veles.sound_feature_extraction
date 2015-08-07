/*! @file lowpass_filter.h
 *  @brief Lowpass filter.
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

#ifndef SRC_TRANSFORMS_LOWPASS_FILTER_H_
#define SRC_TRANSFORMS_LOWPASS_FILTER_H_

#include "src/transforms/iir_filter_base.h"
#include "src/transforms/single_frequency_filter.h"

namespace sound_feature_extraction {
namespace transforms {

using LowpassFilterBase = SingleFrequencyFilter<IIRFilterBase>;

/// @brief Discards the frequencies which are higher than the threshold.
class LowpassFilter : public LowpassFilterBase {
 public:
  TRANSFORM_INTRO("Lowpass", "Lowpass filter. All frequences higher than "
                             "\"frequency\" shall not pass.",
                  LowpassFilter)

 protected:
  virtual std::shared_ptr<IIRFilter> CreateExecutor() const noexcept override;
  virtual void Execute(const std::shared_ptr<IIRFilter>& exec, const float* in,
                       float* out) const override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_LOWPASS_FILTER_H_
