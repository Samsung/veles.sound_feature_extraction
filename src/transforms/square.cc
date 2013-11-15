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
#ifdef SIMD
    for (int j = 0; j < length - FLOAT_STEP + 1; j += FLOAT_STEP) {
      real_multiply(input + j, input + j, output + j);
    }
    for (int j = ((length >> FLOAT_STEP_LOG2) << FLOAT_STEP_LOG2);
        j < length; j++) {
      output[j] = input[j] * input[j];
    }
  } else {
#else
  } {
#endif
    for (int j = 0; j < length; j++) {
      output[j] = input[j] * input[j];
    }
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
