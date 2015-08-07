/*! @file convolution_filter_base.cc
 *  @brief Base class for all IIR filters.
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

#include "src/transforms/iir_filter_base.h"

namespace sound_feature_extraction {
namespace transforms {

RTP(IIRFilterBase, type)
RTP(IIRFilterBase, ripple)
RTP(IIRFilterBase, rolloff)

IIRFilterType Parse(const std::string& value, identity<IIRFilterType>) {
  static const std::unordered_map<std::string, IIRFilterType> map {
    { internal::kIIRFilterTypeBesselStr, IIRFilterType::kBessel },
    { internal::kIIRFilterTypeButterworthStr, IIRFilterType::kButterworth },
    { internal::kIIRFilterTypeChebyshevIStr, IIRFilterType::kChebyshevI },
    { internal::kIIRFilterTypeChebyshevIIStr, IIRFilterType::kChebyshevII },
    { internal::kIIRFilterTypeEllipticStr, IIRFilterType::kElliptic },
    { internal::kIIRFilterTypeLegendreStr, IIRFilterType::kLegendre }
  };
  auto fti = map.find(value);
  if (fti == map.end()) {
    throw InvalidParameterValueException();
  }
  return fti->second;
}

constexpr IIRFilterType IIRFilterBase::kDefaultIIRFilterType;

IIRFilterBase::IIRFilterBase() noexcept
    : type_(kDefaultIIRFilterType),
      ripple_(kDefaultIIRFilterRipple),
      rolloff_(kDefaultIIRFilterRolloff) {
}
ALWAYS_VALID_TP(IIRFilterBase, type)

bool IIRFilterBase::validate_ripple(const float& value) noexcept {
  return value > 0;
}

ALWAYS_VALID_TP(IIRFilterBase, rolloff)

}  // namespace formats
}  // namespace sound_feature_extraction
