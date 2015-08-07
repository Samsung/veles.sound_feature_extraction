/*! @file single_frequency_filter.h
 *  @brief Low- and highpass filters common features.
 *  @author Vadim Markovtsev <v.markovtsev@samsung.com>
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

#ifndef SRC_TRANSFORMS_SINGLE_FREQUENCY_FILTER_H_
#define SRC_TRANSFORMS_SINGLE_FREQUENCY_FILTER_H_

namespace sound_feature_extraction {
namespace transforms {

template <class B>
class SingleFrequencyFilter : public B {
 public:
  TRANSFORM_PARAMETERS_SUPPORT(SingleFrequencyFilter<B>)

  SingleFrequencyFilter() noexcept : frequency_(B::kMinFilterFrequency) {
  }

  TP(frequency, int, B::kMinFilterFrequency, "Filter's cutoff frequency.")
};

template <class B>
bool SingleFrequencyFilter<B>::validate_frequency(const int& value) noexcept {
  return B::ValidateFrequency(value);
}

template <class B>
RTP(SingleFrequencyFilter<B>, frequency)

}  // namespace sound_feature_extraction
}  // namespace transforms

#endif  // SRC_TRANSFORMS_SINGLE_FREQUENCY_FILTER_H_
