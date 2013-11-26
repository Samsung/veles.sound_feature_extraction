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

namespace sound_feature_extraction {
namespace transforms {

Anchor Parse(const std::string value, identity<Anchor>) {
  if (value == internal::kAnchorLeftStr) {
    return Anchor::kLeft;
  }
  if (value == internal::kAnchorRightStr) {
    return Anchor::kRight;
  }
  throw InvalidParameterValueException();
}

constexpr Anchor Selector::kDefaultAnchor;

Selector::Selector()
    : length_(kDefaultLength),
      from_(kDefaultAnchor) {
}

bool Selector::validate_length(const int& value) noexcept {
  return value >= 1;
}

ALWAYS_VALID_TP(Selector, from)

size_t Selector::OnFormatChanged(size_t buffersCount) {
  output_format_->SetSize(std::min(static_cast<size_t>(length_),
                                   input_format_->Size()));
  return buffersCount;
}

void Selector::Do(const float* in, float* out) const noexcept {
  int length = output_format_->Size();
  int offset = (from_ == Anchor::kLeft?
      0 : input_format_->Size() - length);
  if (in != out) {
    memcpy(out, in + offset, length * sizeof(in[0]));
  } else if (from_ == Anchor::kRight) {
    memmove(out, in + offset, length * sizeof(in[0]));
  }
}

RTP(Selector, from)
RTP(Selector, length)
REGISTER_TRANSFORM(Selector);

}  // namespace transforms
}  // namespace sound_feature_extraction
