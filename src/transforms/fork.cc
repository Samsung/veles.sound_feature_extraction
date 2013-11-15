/*! @file fork.cc
 *  @brief Window cloning.
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

namespace sound_feature_extraction {
namespace transforms {

Fork::Fork() : factor_(kDefaultFactor) {
}

bool Fork::validate_factor(const int& value) noexcept {
  return value >= 1;
}

size_t Fork::OnFormatChanged(size_t buffersCount) {
  return factor_ * buffersCount;
}

void Fork::Do(
    const BuffersBase<float*>& in,
    BuffersBase<float*>* out) const noexcept {
  size_t copy_size = input_format_->Size() * sizeof(float);
  for (size_t i = 0, j = 0; i < in.Count(); i++, j += factor_) {
    auto input = in[i];
    for (int k = 0; k < factor_; k++) {
      memcpy((*out)[j + k], input, copy_size);
    }
  }
}

RTP(Fork, factor)
REGISTER_TRANSFORM(Fork);

}  // namespace transforms
}  // namespace sound_feature_extraction
