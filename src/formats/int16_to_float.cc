/*! @file int16_to_float.cc
 *  @brief int16 to float converter.
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
#include <simd/arithmetic-inl.h>

namespace sound_feature_extraction {
namespace formats {

void Int16ToFloatRaw::Do(const int16_t* in,
                         float* out) const noexcept {
  int16_to_float(in, inputFormat_->Size(), out);
}

REGISTER_TRANSFORM(Int16ToFloatRaw);

}  // namespace formats
}  // namespace sound_feature_extraction
