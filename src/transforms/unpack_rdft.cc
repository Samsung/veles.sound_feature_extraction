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

void UnpackRDFT::Initialize() const noexcept {
  realMode_ = inputFormat_->Size() % 2 == 1;
  offset_ = inputFormat_->Size();
  length_ = outputFormat_->Size() - offset_;
}

void UnpackRDFT::Do(const Formats::WindowF& in,
                    Formats::WindowF* out) const noexcept {
  auto input = in.Data.get();
  auto output = out->Data.get();
  if (input != output) {
    memcpy(output, input, offset_ * sizeof(input[0]));
  }
  if (realMode_) {
    rmemcpyf(output + offset_, input + 1, length_);
  } else {
    crmemcpyf(output + offset_, input + 2, length_);
  }
}

REGISTER_TRANSFORM(UnpackRDFT);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
