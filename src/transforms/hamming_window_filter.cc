/*! @file hamming_window_filter.cc
 *  @brief Base class of all filters built on the Hamming window.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/lowpass_filter.h"
#include "src/formats/format_limits.h"
#include "src/primitives/window.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

HammingWindowFilter::HammingWindowFilter(
    const std::unordered_map<std::string, ParameterTraits>&
      supportedParameters) noexcept
: FirFilterBase(supportedParameters) {
}

float HammingWindowFilter::WindowFunction(int index) const noexcept {
  return HammingWindow(index, length_);
}

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
