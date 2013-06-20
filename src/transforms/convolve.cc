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

void ConvolveFilter::CalculateFilter(float*) const noexcept {
}

REGISTER_TRANSFORM(ConvolveFilter);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
