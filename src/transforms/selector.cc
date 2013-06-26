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

namespace SoundFeatureExtraction {
namespace Transforms {

const int Selector::kDefaultLength = 12;
const Selector::Anchor Selector::kDefaultAnchor = ANCHOR_LEFT;

Selector::Selector()
  : length_(kDefaultLength),
    from_(kDefaultAnchor) {
  RegisterSetter("length", [&](const std::string& value) {
    int length = Parse<size_t>("length", value);
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

BuffersCountChange Selector::OnFormatChanged() {
  outputFormat_->SetSize(std::min(length_, inputFormat_->Size()));
  return BuffersCountChange::Identity;
}

void Selector::Do(const float* in,
                  float* out) const noexcept {
  int length = outputFormat_->Size();
  int offset = (from_ == ANCHOR_LEFT? 0 : inputFormat_->Size() - length);
  if (in != out) {
    memcpy(out, in + offset, length * sizeof(in[0]));
  } else if (from_ == ANCHOR_RIGHT) {
    memmove(out, in + offset, length * sizeof(in[0]));
  }
}

REGISTER_TRANSFORM(Selector);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
