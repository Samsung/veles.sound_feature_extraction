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

void SFM::InitializeBuffers(
    const BuffersBase<FixedArray<MEAN_TYPE_COUNT>>& in,
    BuffersBase<float>* buffers) const noexcept {
  buffers->Initialize(in.Size());
}

void SFM::Do(const BuffersBase<FixedArray<MEAN_TYPE_COUNT>>& in,
             BuffersBase<float> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    float gMean = (*in[i])[MEAN_TYPE_GEOMETRIC];
    float aMean = (*in[i])[MEAN_TYPE_ARITHMETIC];
    if (gMean == 0) {
      fprintf(stderr, "Warning: buffer #%zu has geometric mean equal to 0.\n", i);
    }
    if (aMean == 0) {
      fprintf(stderr, "Error: buffer #%zu has arithmetic mean equal to 0.\n", i);
      assert(aMean != 0);
    }
    if (gMean / aMean < 0) {
      fprintf(stderr, "Error: buffer #%zu has geometric and arithmetic means "
                      "of different sign.\n", i);
      assert(gMean / aMean >= 0);
    }
    *(*out)[i] = logf(gMean / aMean);
  }
}

REGISTER_TRANSFORM(SFM);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
