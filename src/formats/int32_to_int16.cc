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
#include <simd/arithmetic-inl.h>

namespace SoundFeatureExtraction {
namespace Formats {

void Int32ToInt16Raw::Do(const Raw32& in,
                         Raw16 *out) const noexcept {
  int32_to_int16(in.Data.get(), inputFormat_->Size(),
                 out->Data.get());
}

void Int32ToInt16Window::Do(const Window32& in,
                            Window16 *out) const noexcept {
    int32_to_int16(in.Data.get(), inputFormat_->Size(),
                   out->Data.get());
}

REGISTER_TRANSFORM(Int32ToInt16Raw);
REGISTER_TRANSFORM(Int32ToInt16Window);

}  // namespace Formats
}  // namespace SoundFeatureExtraction
