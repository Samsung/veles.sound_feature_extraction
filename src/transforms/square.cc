/*! @file template_transform.cc
 *  @brief New file description.
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
#include "src/primitives/arithmetic-inl.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

bool SquareRaw::HasInverse() const noexcept {
  return true;
}

void SquareRaw::OnInputFormatChanged() {
  outputFormat_->SetSize(inputFormat_->Size());
  outputFormat_->SetSamplingRate(inputFormat_->SamplingRate());
}

void SquareRaw::InitializeBuffers(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Raw32>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size()
#ifdef __AVX__
                      , in[0]->AlignmentOffset()
#endif
                      );  // NOLINT(whitespace/parens)
}

void SquareRaw::Do(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Raw32>* out) const noexcept {
  assert(!IsInverse() && "Not implemented yet");
  int arrayLength = outputFormat_->Size();
  for (size_t i = 0; i < in.Size(); i++) {
    auto inArray = in[i]->Data.get();
    auto outArray = (*out)[i]->Data.get();

#ifdef SIMD
    int startIndex = 0;
#ifdef __AVX__
    startIndex = align_complement_i16(inArray);

    for (int j = 0; j < startIndex; j++) {
      outArray[j] = inArray[j] * inArray[j];
    }
#endif
    for (int j = startIndex; j < arrayLength - INT16MUL_STEP + 1;
        j += INT16MUL_STEP) {
      int16_multiply(inArray + j, inArray + j, outArray + j);
    }

    for (int j = startIndex + (((arrayLength - startIndex)
            >> INT16MUL_STEP_LOG2) << INT16MUL_STEP_LOG2);
         j < arrayLength; j++) {
      outArray[j] = inArray[j] * inArray[j];
    }
#else
    for (int j = 0; j < arrayLength; j++) {
      outArray[j] = inArray[j] * inArray[j];
    }
#endif
  }
}

bool SquareWindow::HasInverse() const noexcept {
  return true;
}

void SquareWindow::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size());
}

void SquareWindow::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
  assert(!IsInverse() && "Not implemented yet");
  int arrayLength = outputFormat_->Size();
  for (size_t i = 0; i < in.Size(); i++) {
    auto inArray = in[i]->Data.get();
    auto outArray = (*out)[i]->Data.get();
#ifdef SIMD
    for (int j = 0; j < arrayLength - FLOAT_STEP + 1; j += FLOAT_STEP) {
      real_multiply(inArray + j, inArray + j, outArray + j);
    }
    for (int j = ((arrayLength >> FLOAT_STEP_LOG2) << FLOAT_STEP_LOG2);
        j < arrayLength; j++) {
      outArray[j] = inArray[j] * inArray[j];
    }
#else
    for (int j = 0; j < arrayLength; j++) {
      outArray[j] = inArray[j] * inArray[j];
    }
#endif
  }
}

REGISTER_TRANSFORM(SquareRaw);
REGISTER_TRANSFORM(SquareWindow);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
