/*! @file square.cc
 *  @brief Square the input signal.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/square.h"
#include <simd/arithmetic-inl.h>

namespace sound_feature_extraction {
namespace transforms {

void Square::Do(const float* in, float* out) const noexcept {
  Do(use_simd(), in, output_format_->Size(), out);
}

void Square::Do(bool simd, const float* input, int length,
                float* output) noexcept {
  if (simd) {
    real_multiply_array(input, input, length, output);
  } else {
    real_multiply_array_na(input, input, length, output);
  }
}

void SquareInverse::Do(const float* in UNUSED,
                       float* out UNUSED) const noexcept {
  assert(false && "Not implemented yet");
}

REGISTER_TRANSFORM(Square);
REGISTER_TRANSFORM(SquareInverse);

}  // namespace transforms
}  // namespace sound_feature_extraction
