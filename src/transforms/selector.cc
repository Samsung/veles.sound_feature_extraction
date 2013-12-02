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
#include <simd/memory.h>

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
      select_(kDefaultSelect),
      from_(kDefaultAnchor) {
}

bool Selector::validate_length(const int& value) noexcept {
  return value >= 0;
}

bool Selector::validate_select(const int& value) noexcept {
  return value >= 0;
}

ALWAYS_VALID_TP(Selector, from)

size_t Selector::OnFormatChanged(size_t buffersCount) {
  if (length_ == 0) {
    length_ = input_format_->Size();
  }
  if (select_ == 0) {
    select_ = length_;
  }
  output_format_->SetSize(length_);
  if (select_ > static_cast<int>(input_format_->Size())) {
    throw InvalidParameterValueException("select", std::to_string(select_),
                                         HostName());
  }
  return buffersCount;
}

void Selector::Do(const float* in, float* out) const noexcept {
  switch (from_) {
    case Anchor::kLeft:
      memcpy(out, in, select_ * sizeof(in[0]));
      memsetf(out + select_, 0.f, length_ - select_);
      break;
    case Anchor::kRight:
      memcpy(out + length_ - select_, in + input_format_->Size() - select_,
             select_ * sizeof(in[0]));
      memsetf(out, 0.f, length_ - select_);
      break;
  }
}

RTP(Selector, from)
RTP(Selector, length)
RTP(Selector, select)
REGISTER_TRANSFORM(Selector);

}  // namespace transforms
}  // namespace sound_feature_extraction
