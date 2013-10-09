/*! @file convolve.cc
 *  @brief Convolve a raw signal with a window function.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/convolve.h"
#include <math.h>

namespace SoundFeatureExtraction {
namespace Transforms {

ConvolveFilter::ConvolveFilter() noexcept : window_(WINDOW_TYPE_RECTANGULAR) {
  RegisterSetter("window", [&](const std::string& value) {
    auto wti = kWindowTypeMap.find(value);
    if (wti == kWindowTypeMap.end()) {
      return false;
    }
    window_ = wti->second;
    return true;
  });
}

void ConvolveFilter::CalculateFilter(float* window) const noexcept {
  for (int i = 0; i < length(); i++) {
    window[i] = WindowElement(window_, length(), i);
  }
}

REGISTER_TRANSFORM(ConvolveFilter);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
