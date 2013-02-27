/*! @file int16_to_float.cc
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

#include "src/formats/int16_to_float.h"
#include "src/primitives/arithmetic-inl.h"

namespace SpeechFeatureExtraction {
namespace Formats {

void Int16ToFloatRaw::TypeSafeDo(const BuffersBase<Raw16>& in,
                                 BuffersBase<RawF> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    int16_to_float(in[i]->Data.get(), inputFormat_.Size(),
                   (*out)[i]->Data.get());
  }
}

void Int16ToFloatWindow::TypeSafeDo(
    const BuffersBase<Window16>& in,
    BuffersBase<WindowF> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    int16_to_float(in[i]->Data.get(), inputFormat_.SamplesCount(),
                   (*out)[i]->Data.get());
  }
}

REGISTER_TRANSFORM(Int16ToFloatRaw);
REGISTER_TRANSFORM(Int16ToFloatWindow);

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
