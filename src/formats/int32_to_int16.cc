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

namespace sound_feature_extraction {
namespace formats {

void Int32ToInt16Raw::Do(const int32_t* in,
                         int16_t* out) const noexcept {
  int32_to_int16(in, inputFormat_->Size(), out);
}

REGISTER_TRANSFORM(Int32ToInt16Raw);

}  // namespace formats
}  // namespace sound_feature_extraction
