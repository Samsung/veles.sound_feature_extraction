/*! @file selector.cc
 *  @brief Select the specified part of input.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/selector.h"
#include <algorithm>
#include <string>

namespace SpeechFeatureExtraction {
namespace Transforms {

Selector::Selector()
  : length_(12),
    from_(ANCHOR_RIGHT) {
  RegisterSetter("length", [&](const std::string& value) {
    int length = Parse<int>("length", value);
    if (length < 1) {
      return false;
    }
    length_ = length;
    return true;
  });
  RegisterSetter("from", [&](const std::string& value) {
    if (value == "left") {
      from_ = ANCHOR_LEFT;
    } else if (value == "right") {
      from_ = ANCHOR_RIGHT;
    } else {
      return false;
    }
    return true;
  });
}

void Selector::OnFormatChanged() {
  outputFormat_->SetSize(
      std::min(length_, static_cast<int>(inputFormat_->Size())));
}

void Selector::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), outputFormat_->Size());
}

void Selector::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
  int length = outputFormat_->Size();
  for (size_t i = 0; i < in.Size(); i++) {
    auto input = in[i]->Data.get();
    auto output = (*out)[i]->Data.get();
    if (input != output) {
      memcpy(output,
             input + (from_ == ANCHOR_LEFT? 0 : inputFormat_->Size() - length),
             length * sizeof(input[0]));
    } else {
      memmove(output,
              input + (from_ == ANCHOR_LEFT? 0 : inputFormat_->Size() - length),
              length * sizeof(input[0]));
    }
  }
}

REGISTER_TRANSFORM(Selector);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction