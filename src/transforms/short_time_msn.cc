/*! @file short_time_msn.cc
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

#include <simd/arithmetic-inl.h>
#include "src/transforms/short_time_msn.h"

namespace sound_feature_extraction {
namespace transforms {

ShortTimeMeanScaleNormalization::ShortTimeMeanScaleNormalization()
    : length_(kDefaultLength) {
}

bool ShortTimeMeanScaleNormalization::validate_length(
    const int& value) noexcept {
  return value >= 2;
}

void ShortTimeMeanScaleNormalization::Do(
    const BuffersBase<float*>& in,
    BuffersBase<float*>* out) const noexcept {
  int back = length_ / 2;
  int front = length_ - back;
  for (size_t i = 0; i < in.Count(); i++) {
    for (int j = 0; j < static_cast<int>(input_format_->Size()); j++) {
      int len = length_;
      int backind = i - back;
      if (backind < 0) {
        len += backind;
        backind = 0;
      }
      int frontind = i + front;
      if (frontind > static_cast<int>(in.Count())) {
        len += in.Count() - frontind;
        frontind = static_cast<int>(in.Count());
      }
      float sum = 0.f;
      float thisval = in[i][j];
      float min = thisval;
      float max = thisval;
      for (int k = backind; k < frontind; k++) {
        float val = in[k][j];
        sum += val;
        if (min > val) {
          min = val;
        } else if (max < val) {
          max = val;
        }
      }
      if (max - min > 0) {
        (*out)[i][j] = (thisval - sum / len) / (max - min);
      } else {
        (*out)[i][j] = 0;
      }
    }
  }
}

RTP(ShortTimeMeanScaleNormalization, length)
REGISTER_TRANSFORM(ShortTimeMeanScaleNormalization);

}  // namespace transforms
}  // namespace sound_feature_extraction
