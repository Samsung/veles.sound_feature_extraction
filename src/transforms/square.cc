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

void SquareRaw::OnInputFormatChanged() {
  outputFormat_->SetSize(inputFormat_->Size());
  outputFormat_->SetSamplingRate(inputFormat_->SamplingRate());
}

void SquareRaw::InitializeBuffers(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Raw32>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size(),
                      in[0].AlignmentOffset());
}

void SquareRaw::InitializeBuffers(
    const BuffersBase<Formats::Raw32>& in,
    BuffersBase<Formats::Raw16>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size(),
                      in[0].AlignmentOffset());
}

void SquareRaw::Do(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Raw32>* out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    Do(UseSimd(), in[i].Data.get(), outputFormat_->Size(), (*out)[i].Data.get());
  }
}

void SquareRaw::Do(
    const BuffersBase<Formats::Raw32>& in UNUSED,
    BuffersBase<Formats::Raw16>* out UNUSED) const noexcept {
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

void SquareWindow::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size());
}

void SquareWindow::Do(const Formats::WindowF& in,
                      Formats::WindowF* out) const noexcept {
  assert(!IsInverse() && "Not implemented yet");
  Do(UseSimd(), in.Data.get(), outputFormat_->Size(), out->Data.get());
}

void SquareWindow::Do(bool simd, const float* input, int length,
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
REGISTER_TRANSFORM(SquareWindow);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
