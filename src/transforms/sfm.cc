/*! @file sfm.cc
 *  @brief Arithmetic and geometric sfms calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/sfm.h"
#include <math.h>

namespace SoundFeatureExtraction {
namespace Transforms {

using Formats::FixedArray;

void SFM::Do(const FixedArray<MEAN_TYPE_COUNT>& in,
             float* out) const noexcept {
  float gMean = in[MEAN_TYPE_GEOMETRIC];
  float aMean = in[MEAN_TYPE_ARITHMETIC];
  if (gMean == 0) {
    ERR("Buffer has geometric mean equal to 0.");
    *out = 0;
    return;
  }
  if (aMean == 0) {
    ERR("Buffer has arithmetic mean equal to 0.");
    *out = 0;
    return;
  }
  if (gMean / aMean < 0) {
    ERR("Buffer has geometric and arithmetic means of different sign.");
    *out = 0;
    return;
  }
  *out = logf(gMean / aMean);
}

REGISTER_TRANSFORM(SFM);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
