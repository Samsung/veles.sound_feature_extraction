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
#include <cmath>

namespace sound_feature_extraction {
namespace transforms {

ConvolveFilter::ConvolveFilter() noexcept : window_(kDefaultWindowType) {
}

ALWAYS_VALID_TP(ConvolveFilter, window)

void ConvolveFilter::CalculateFilter(float* window) const noexcept {
  for (int i = 0; i < length(); i++) {
    window[i] = WindowElement(window_, length(), i);
  }
}

RTP(ConvolveFilter, window)
REGISTER_TRANSFORM(ConvolveFilter);

}  // namespace transforms
}  // namespace sound_feature_extraction
