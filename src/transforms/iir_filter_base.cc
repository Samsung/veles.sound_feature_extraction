/*! @file convolution_filter_base.cc
 *  @brief Base class for all IIR filters.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/iir_filter_base.h"

namespace sound_feature_extraction {
namespace transforms {

RTP(IIRFilterBase, type)
RTP(IIRFilterBase, ripple)
RTP(IIRFilterBase, rolloff)

IIRFilterType Parse(const std::string& value, identity<IIRFilterType>) {
  static const std::unordered_map<std::string, IIRFilterType> map {
    { internal::kIIRFilterTypeBesselStr, kIIRFilterTypeBessel },
    { internal::kIIRFilterTypeButterworthStr, kIIRFilterTypeButterworth },
    { internal::kIIRFilterTypeChebyshevIStr, kIIRFilterTypeChebyshevI },
    { internal::kIIRFilterTypeChebyshevIIStr, kIIRFilterTypeChebyshevII },
    { internal::kIIRFilterTypeEllipticStr, kIIRFilterTypeElliptic },
    { internal::kIIRFilterTypeLegendreStr, kIIRFilterTypeLegendre }
  };
  auto fti = map.find(value);
  if (fti == map.end()) {
    throw InvalidParameterValueException();
  }
  return fti->second;
}

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
