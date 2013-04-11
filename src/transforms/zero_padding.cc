/*! @file zero_padding.cc
 *  @brief Pad signal with zeros to make it's length a power of 2.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/zero_padding.h"
#include "src/primitives/arithmetic-inl.h"

namespace SoundFeatureExtraction {
namespace Transforms {

void ZeroPadding::OnFormatChanged() {
  int length = inputFormat_->Size();
  if ((length & (length - 1)) == 0) {
    outputFormat_->SetSize(length);
  } else {
    outputFormat_->SetSize(next_highest_power_of_2(length));
  }
}

void ZeroPadding::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), outputFormat_->Size());
}

void ZeroPadding::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    auto input = in[i]->Data.get();
    auto output = (*out)[i]->Data.get();
    if (input != output) {
      memcpy(output, input, inputFormat_->Size() * sizeof(input[0]));
    }
    memsetf(output + inputFormat_->Size(),
            outputFormat_->Size() - inputFormat_->Size(),
            .0f);
  }
}

REGISTER_TRANSFORM(ZeroPadding);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
