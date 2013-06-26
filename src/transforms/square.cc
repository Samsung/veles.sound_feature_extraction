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

namespace SoundFeatureExtraction {
namespace Transforms {

BuffersCountChange SquareRaw::OnInputFormatChanged() {
  outputFormat_->SetSize(inputFormat_->Size());
  outputFormat_->SetSamplingRate(inputFormat_->SamplingRate());
  return BuffersCountChange::Identity;
}

void SquareRaw::Do(const int16_t* in,
                   int32_t* out) const noexcept {
  Do(UseSimd(), in, outputFormat_->Size(), out);
}

void SquareRaw::DoInverse(const int32_t* in UNUSED,
                          int16_t* out UNUSED) const noexcept {
  assert(false && "Not implemented yet");
}

void SquareRaw::Do(bool simd, const int16_t* input, int length,
                   int32_t* output) noexcept {
  if (simd) {
#ifdef SIMD
    int startIndex = 0;
#ifdef __AVX__
    startIndex = align_complement_i16(input);

    for (int j = 0; j < startIndex; j++) {
      output[j] = input[j] * input[j];
    }
#endif
    for (int j = startIndex; j < length - INT16MUL_STEP + 1;
        j += INT16MUL_STEP) {
      int16_multiply(input + j, input + j, output + j);
    }

    for (int j = startIndex + (((length - startIndex)
            >> INT16MUL_STEP_LOG2) << INT16MUL_STEP_LOG2);
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

void SquareF::Do(const float* in,
                      float* out) const noexcept {
  Do(UseSimd(), in, outputFormat_->Size(), out);
}

void SquareF::DoInverse(const float* in UNUSED,
                        float* out UNUSED) const noexcept {
  assert(false && "Not implemented yet");
}

void SquareF::Do(bool simd, const float* input, int length,
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

REGISTER_TRANSFORM(SquareRaw);
REGISTER_TRANSFORM(SquareF);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
