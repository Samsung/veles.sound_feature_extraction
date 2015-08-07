/*! @file iir_filter_base.h
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

#ifndef SRC_TRANSFORMS_IIR_FILTER_BASE_H_
#define SRC_TRANSFORMS_IIR_FILTER_BASE_H_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wsequence-point"
#include <DspFilters/Dsp.h>
#pragma GCC diagnostic pop
#include "src/transforms/filter_base.h"

namespace sound_feature_extraction {
namespace transforms {

enum class IIRFilterType {
  kBessel,
  kButterworth,
  kChebyshevI,
  kChebyshevII,
  kElliptic,
  kLegendre
};

namespace internal {
constexpr const char* kIIRFilterTypeBesselStr = "bessel";
constexpr const char* kIIRFilterTypeButterworthStr = "butterworth";
constexpr const char* kIIRFilterTypeChebyshevIStr = "chebyshevI";
constexpr const char* kIIRFilterTypeChebyshevIIStr = "chebyshevII";
constexpr const char* kIIRFilterTypeEllipticStr = "elliptic";
constexpr const char* kIIRFilterTypeLegendreStr = "legendre";
}

IIRFilterType Parse(const std::string& value, identity<IIRFilterType>);

}  // namespace transforms
}  // namespace sound_feature_extraction

namespace std {
  using sound_feature_extraction::transforms::IIRFilterType;

  inline string
  to_string(const IIRFilterType& type) noexcept {
    switch (type) {
      case IIRFilterType::kBessel:
        return sound_feature_extraction::transforms::
            internal::kIIRFilterTypeBesselStr;
      case IIRFilterType::kButterworth:
        return sound_feature_extraction::transforms::
            internal::kIIRFilterTypeButterworthStr;
      case IIRFilterType::kChebyshevI:
        return sound_feature_extraction::transforms::
            internal::kIIRFilterTypeChebyshevIStr;
      case IIRFilterType::kChebyshevII:
        return sound_feature_extraction::transforms::
            internal::kIIRFilterTypeChebyshevIIStr;
      case IIRFilterType::kElliptic:
        return sound_feature_extraction::transforms::
            internal::kIIRFilterTypeEllipticStr;
      case IIRFilterType::kLegendre:
        return sound_feature_extraction::transforms::
            internal::kIIRFilterTypeLegendreStr;
    }
    return "";
  }
}  // namespace std

namespace sound_feature_extraction {
namespace transforms {

typedef Dsp::Cascade IIRFilter;

class IIRFilterBase : public FilterBase<IIRFilter> {
 public:
  IIRFilterBase() noexcept;

  TRANSFORM_PARAMETERS_SUPPORT(IIRFilterBase)

  TP(type, IIRFilterType, kDefaultIIRFilterType,
     "IIR filter type (bessel, butterworth, etc.).")
  TP(ripple, float, kDefaultIIRFilterRipple,
     "Ripple level in dB (used by a subset of filter types).")
  TP(rolloff, float, kDefaultIIRFilterRolloff,
     "Rolloff level in dB (used by a subset of filter types).")

 protected:
  template <class F>
  void Execute(const std::shared_ptr<F>& exec, const float* in,
               float* out) const {
    memcpy(out, in, input_format_->UnalignedSizeInBytes());
    auto ptr = std::const_pointer_cast<F>(exec);
    ptr->reset();
    ptr->process(input_format_->Size(), &out);
  }

  static constexpr IIRFilterType kDefaultIIRFilterType =
      IIRFilterType::kChebyshevII;
  static constexpr float kDefaultIIRFilterRipple = 1;
  static constexpr float kDefaultIIRFilterRolloff = 0;
};

}  // namespace transforms
}  // namespace sound_feature_extraction

#endif  // SRC_TRANSFORMS_IIR_FILTER_BASE_H_
