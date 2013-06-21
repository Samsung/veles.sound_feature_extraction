/*! @file float_to_int16.cc
 *  @brief float to int16 converter.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/formats/float_to_int16.h"
#include <simd/arithmetic-inl.h>

namespace SoundFeatureExtraction {
namespace Formats {

void FloatToInt16Raw::Do(const RawF& in,
                         Raw16 *out) const noexcept {
  float_to_int16(in.Data.get(), inputFormat_->Size(),
                 out->Data.get());
}

void FloatToInt16Window::Do(const WindowF& in,
                            Window16 *out) const noexcept {
  float_to_int16(in.Data.get(), inputFormat_->Size(),
                 out->Data.get());
}

REGISTER_TRANSFORM(FloatToInt16Raw);
REGISTER_TRANSFORM(FloatToInt16Window);

}  // namespace Formats
}  // namespace SoundFeatureExtraction
