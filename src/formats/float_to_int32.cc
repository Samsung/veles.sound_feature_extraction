/*! @file float_to_int32.cc
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/formats/float_to_int32.h"
#include "src/primitives/arithmetic.h"

namespace SpeechFeatureExtraction {
namespace Formats {

void FloatToInt32Raw::TypeSafeInitializeBuffers(
    const BuffersBase<RawF>& in,
    BuffersBase<Raw32>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_.Size());
}

void FloatToInt32Raw::TypeSafeDo(const BuffersBase<RawF>& in,
                                 BuffersBase<Raw32> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    float_to_int32(in[i]->Data.get(), inputFormat_.Size(),
                   (*out)[i]->Data.get());
  }
}

void FloatToInt32Window::TypeSafeInitializeBuffers(
    const BuffersBase<WindowF>& in,
    BuffersBase<Window32>* buffers) const noexcept {
  buffers->Initialize(in.Size());
}

void FloatToInt32Window::TypeSafeDo(
    const BuffersBase<WindowF>& in,
    BuffersBase<Window32> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    float_to_int32(in[i]->Chunk, inputFormat_.SamplesCount(),
                   (*out)[i]->Chunk);
  }
}

REGISTER_TRANSFORM(FloatToInt32Raw);
REGISTER_TRANSFORM(FloatToInt32Window);

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
