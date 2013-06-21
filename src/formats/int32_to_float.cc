/*! @file int32_to_float.cc
 *  @brief int32 to float converter.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/formats/int32_to_float.h"
#include <simd/arithmetic-inl.h>

namespace SoundFeatureExtraction {
namespace Formats {

void Int32ToFloatRaw::Do(const Raw32& in,
                         RawF *out) const noexcept {
  int32_to_float(in.Data.get(), inputFormat_->Size(),
                 out->Data.get());
}

void Int32ToFloatWindow::Do(const Window32& in,
                            WindowF* out) const noexcept {
  int32_to_float(in.Data.get(), inputFormat_->Size(),
                 out->Data.get());
}

REGISTER_TRANSFORM(Int32ToFloatRaw);
REGISTER_TRANSFORM(Int32ToFloatWindow);

}  // namespace Formats
}  // namespace SoundFeatureExtraction
