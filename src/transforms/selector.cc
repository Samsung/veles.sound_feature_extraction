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
#include <string>

namespace SpeechFeatureExtraction {
namespace Transforms {

Selector::Selector()
  : UniformFormatTransform(SupportedParameters()),
    length_(12),
    from_(ANCHOR_RIGHT) {
}

void Selector::SetParameter(const std::string& name,
                            const std::string& value) {
  if (name == "length") {
    int length = Parse<int>(name, value);
    if (length < 1) {
      throw InvalidParameterValueException(name, value, Name());
    }
    length_ = length;
  } else if (name == "from") {
    if (value == "left") {
      from_ = ANCHOR_LEFT;
    } else if (value == "right") {
      from_ = ANCHOR_RIGHT;
    } else {
      throw InvalidParameterValueException(name, value, Name());
    }
  }
}

void Selector::OnFormatChanged() {
  outputFormat_->SetSize(
      std::min(length_, static_cast<int>(inputFormat_->Size())));
}

void Selector::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), outputFormat_->Size());
}

void Selector::TypeSafeDo(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF> *out) const noexcept {
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
