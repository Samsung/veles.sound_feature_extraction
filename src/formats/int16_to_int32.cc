/*! @file int16_to_int32.cc
 *  @brief int16 to int32 converter.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/formats/int16_to_int32.h"
#include <simd/arithmetic-inl.h>

namespace SoundFeatureExtraction {
namespace Formats {

void Int16ToInt32Raw::Do(const BuffersBase<Raw16>& in,
                         BuffersBase<Raw32> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    int16_to_int32(in[i].Data.get(), inputFormat_->Size(),
                   (*out)[i].Data.get());
  }
}

void Int16ToInt32Window::Do(const BuffersBase<Window16>& in,
                            BuffersBase<Window32> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    int16_to_int32(in[i].Data.get(), inputFormat_->Size(),
                   (*out)[i].Data.get());
  }
}

REGISTER_TRANSFORM(Int16ToInt32Raw);
REGISTER_TRANSFORM(Int16ToInt32Window);

}  // namespace Formats
}  // namespace SoundFeatureExtraction
