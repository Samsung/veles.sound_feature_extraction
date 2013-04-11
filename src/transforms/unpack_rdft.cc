/*! @file unpack_rdft.cc
 *  @brief Unpacks data after applying RDFT.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/unpack_rdft.h"

namespace SoundFeatureExtraction {
namespace Transforms {

void UnpackRDFT::OnFormatChanged() {
  if (inputFormat_->Size() % 2 == 1) {
    outputFormat_->SetSize((inputFormat_->Size() - 1) * 2);
  } else {
    outputFormat_->SetSize((inputFormat_->Size() - 2) * 2);
  }
}

void UnpackRDFT::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), outputFormat_->Size());
}

void UnpackRDFT::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
  bool realMode = inputFormat_->Size() % 2 == 1;
  size_t offset = inputFormat_->Size();
  size_t length = outputFormat_->Size() - offset;
  for (size_t i = 0; i < in.Size(); i++) {
    auto input = in[i]->Data.get();
    auto output = (*out)[i]->Data.get();
    if (input != output) {
      memcpy(output, input, offset * sizeof(input[0]));
    }
    if (realMode) {
      rmemcpyf(output + offset, input + 1, length);
    } else {
      crmemcpyf(output + offset, input + 2, length);
    }
  }
}

REGISTER_TRANSFORM(UnpackRDFT);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
