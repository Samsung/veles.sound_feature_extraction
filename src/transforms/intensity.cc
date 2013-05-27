/*! @file intensity.cc
 *  @brief Sound intensity calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/intensity.h"
#include <math.h>
#include "src/primitives/energy.h"

namespace SoundFeatureExtraction {
namespace Transforms {

void Intensity::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<float>* buffers) const noexcept {
  buffers->Initialize(in.Size());
}

void Intensity::Do(const Formats::WindowF& in,
                   float* out) const noexcept {
  int length = inputFormat_->Size();
  *out = logf(calculate_energy(true, in.Data.get(), length));
}

REGISTER_TRANSFORM(Intensity);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
