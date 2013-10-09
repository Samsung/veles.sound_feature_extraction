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

namespace SoundFeatureExtraction {
namespace Transforms {

const std::unordered_map<std::string, IIRFilterType>
IIRFilterBase::kFilterTypeMap {
  { "bessel", kIIRFilterTypeBessel },
  { "butterworth", kIIRFilterTypeButterworth },
  { "chebyshevI", kIIRFilterTypeChebyshevI },
  { "chebyshevII", kIIRFilterTypeChebyshevII },
  { "elliptic", kIIRFilterTypeElliptic },
  { "legendre", kIIRFilterTypeLegendre }
};

IIRFilterBase::IIRFilterBase() noexcept
    : type_(kDefaultIIRFilterType),
      ripple_(kDefaultIIRFilterRipple),
      rolloff_(kDefaultIIRFilterRolloff) {
  RegisterSetter("type", [&](const std::string& value) {
    auto fti = kFilterTypeMap.find(value);
    if (fti == kFilterTypeMap.end()) {
      return false;
    }
    type_ = fti->second;
    return true;
  });
  RegisterSetter("ripple", [&](const std::string& value) {
    float pv = Parse<float>("ripple", value);
    if (pv <= 0) {
      return false;
    }
    ripple_ = pv;
    return true;
  });
  RegisterSetter("rolloff", [&](const std::string& value) {
    float pv = Parse<float>("rolloff", value);
    rolloff_ = pv;
    return true;
  });
}

IIRFilterType IIRFilterBase::type() const {
  return type_;
}

float IIRFilterBase::ripple() const {
  return ripple_;
}

float IIRFilterBase::rolloff() const {
  return rolloff_;
}

}  // namespace Formats
}  // namespace SoundFeatureExtraction
