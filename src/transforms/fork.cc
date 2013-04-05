/*! @file fork.cc
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

#include "src/transforms/fork.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

const int Fork::kDefaultFactor = 4;
const std::string Fork::kCloningFactorAttributeName = "CloningFactor";

Fork::Fork() : factor_(kDefaultFactor) {
  RegisterSetter("factor", [&](const std::string& value) {
    int pv = Parse<int>("factor", value);
    if (pv < 1) {
      return false;
    }
    factor_ = pv;
    return true;
  });
}

void Fork::OnFormatChanged() {
  int prevFactor = 1;
  if (outputFormat_->HasAttribute(kCloningFactorAttributeName)) {
    prevFactor = outputFormat_->GetAttributeValue<int>(
        kCloningFactorAttributeName);
  }
  outputFormat_->SetAttributeValue(kCloningFactorAttributeName,
                                   prevFactor * factor_);
}

void Fork::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size() * factor_, inputFormat_->Size());
}

void Fork::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
  size_t copy_size = inputFormat_->Size() * sizeof(float);
  for (size_t i = 0, j = 0; i < in.Size(); i++, j += factor_) {
    auto input = in[i]->Data.get();
    for (int k = 0; k < factor_; k++) {
      memcpy((*out)[j + k]->Data.get(), input, copy_size);
    }
  }
}

REGISTER_TRANSFORM(Fork);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
