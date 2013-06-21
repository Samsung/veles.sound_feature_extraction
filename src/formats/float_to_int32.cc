/*! @file float_to_int32.cc
 *  @brief float to int32 converter.
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
#include <simd/arithmetic-inl.h>

namespace SoundFeatureExtraction {
namespace Formats {

void FloatToInt32Raw::Do(const RawF& in,
                         Raw32 *out) const noexcept {
  float_to_int32(in.Data.get(), inputFormat_->Size(),
                 out->Data.get());
}

void FloatToInt32Window::Do(const WindowF& in,
                            Window32 *out) const noexcept {
  float_to_int32(in.Data.get(), inputFormat_->Size(),
                 out->Data.get());
}

REGISTER_TRANSFORM(FloatToInt32Raw);
REGISTER_TRANSFORM(FloatToInt32Window);

}  // namespace Formats
}  // namespace SoundFeatureExtraction
