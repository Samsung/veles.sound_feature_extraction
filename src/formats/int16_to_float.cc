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

namespace SoundFeatureExtraction {
namespace Formats {

void Int16ToFloatRaw::Do(const Raw16& in,
                         RawF *out) const noexcept {
  int16_to_float(in.Data.get(), inputFormat_->Size(),
                 out->Data.get());
}

void Int16ToFloatWindow::Do(const Window16& in,
                            WindowF* out) const noexcept {
  int16_to_float(in.Data.get(), inputFormat_->Size(),
                 out->Data.get());
}

REGISTER_TRANSFORM(Int16ToFloatRaw);
REGISTER_TRANSFORM(Int16ToFloatWindow);

}  // namespace Formats
}  // namespace SoundFeatureExtraction
