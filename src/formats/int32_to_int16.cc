/*! @file int32_to_int16.cc
 *  @brief int32 to int16 converter.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/formats/int32_to_int16.h"
#include "src/primitives/arithmetic-inl.h"

namespace SoundFeatureExtraction {
namespace Formats {

void Int32ToInt16Raw::Do(const BuffersBase<Raw32>& in,
                                 BuffersBase<Raw16> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    int32_to_int16(in[i]->Data.get(), inputFormat_->Size(),
                   (*out)[i]->Data.get());
  }
}

void Int32ToInt16Window::Do(
    const BuffersBase<Window32>& in,
    BuffersBase<Window16> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    int32_to_int16(in[i]->Data.get(), inputFormat_->Size(),
                   (*out)[i]->Data.get());
  }
}

REGISTER_TRANSFORM(Int32ToInt16Raw);
REGISTER_TRANSFORM(Int32ToInt16Window);

}  // namespace Formats
}  // namespace SoundFeatureExtraction
