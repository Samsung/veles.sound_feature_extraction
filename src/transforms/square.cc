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

SquareRaw::SquareRaw()
: TransformBase(SupportedParameters()) {
}

bool SquareRaw::HasInverse() const noexcept {
  return true;
}

void SquareRaw::OnInputFormatChanged() {
  outputFormat_->SetSize(inputFormat_->Size());
  outputFormat_->SetSamplingRate(inputFormat_->SamplingRate());
}

void SquareRaw::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Raw32>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size()
#ifdef __AVX__
                      , in[0]->AlignmentOffset()
#endif
                      );  // NOLINT(whitespace/parens)
}

void SquareRaw::TypeSafeDo(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Raw32>* out) const noexcept {
  assert(!IsInverse() && "Not implemented yet");
  int arrayLength = outputFormat_->Size();
  for (size_t i = 0; i < in.Size(); i++) {
    auto inArray = in[i]->Data.get();
    auto outArray = (*out)[i]->Data.get();
#ifdef __AVX__
    int startIndex = align_complement_i16(inArray);

    for (int j = 0; j < startIndex; j++) {
      outArray[j] = inArray[j] * inArray[j];
    }

#ifdef __AVX2__
    for (int j = startIndex; j < arrayLength - 15; j += 16) {
      int16_multiply(inArray + j, inArray + j, outArray + j);
    }

    for (int j = startIndex + (((arrayLength - startIndex) >> 4) << 4);
         j < arrayLength; j++) {
      outArray[j] = inArray[j] * inArray[j];
    }
#else
    for (int j = startIndex; j < arrayLength - 7; j += 8) {
      int16_multiply(inArray + j, inArray + j, outArray + j);
    }

    for (int j = startIndex + (((arrayLength - startIndex) >> 3) << 3);
         j < arrayLength; j++) {
      outArray[j] = inArray[j] * inArray[j];
    }
#endif
#elif defined(NEON)
    for (int j = 0; j < arrayLength - 3; j += 4) {
      int16_multiply(inArray + j, inArray + j, outArray + j);
    }

    for (int j = ((arrayLength >> 2) << 2); j < arrayLength; j++) {
      outArray[j] = inArray[j] * inArray[j];
    }
#else
    for (int j = 0; j < arrayLength; j++) {
      outArray[j] = inArray[j] * inArray[j];
    }
#endif
  }
}

SquareWindow::SquareWindow()
: UniformFormatTransform(SupportedParameters()) {
}

bool SquareWindow::HasInverse() const noexcept {
  return true;
}

void SquareWindow::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size());
}

void SquareWindow::TypeSafeDo(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
  assert(!IsInverse() && "Not implemented yet");
  int arrayLength = outputFormat_->Size();
  for (size_t i = 0; i < in.Size(); i++) {
    auto inArray = in[i]->Data.get();
    auto outArray = (*out)[i]->Data.get();
#ifdef __AVX__
    for (int j = 0; j < arrayLength - 7; j += 8) {
      real_multiply(inArray + j, inArray + j, outArray + j);
    }
    for (int j = ((arrayLength >> 3) << 3); j < arrayLength; j++) {
      outArray[j] = inArray[j] * inArray[j];
    }
#elif defined(NEON)
    for (int j = 0; j < arrayLength - 3; j += 4) {
      real_multiply(inArray + j, inArray + j, outArray + j);
    }
    for (int j = ((arrayLength >> 2) << 2); j < arrayLength; j++) {
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
