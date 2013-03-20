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
#include <string>
#include "src/primitives/energy.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

void Intensity::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<float>* buffers) const noexcept {
  buffers->Initialize(in.Size());
}

void Intensity::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<float> *out) const noexcept {
  int length = inputFormat_->Size();
  for (size_t i = 0; i < in.Size(); i++) {
    *(*out)[i] = logf(calculate_energy(in[i]->Data.get(), length));
  }
}

REGISTER_TRANSFORM(Intensity);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
