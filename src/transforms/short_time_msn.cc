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

namespace SoundFeatureExtraction {
namespace Transforms {

ShortTimeMeanScaleNormalization::ShortTimeMeanScaleNormalization()
    : length_(kDefaultLength) {
  RegisterSetter("length", [&](const std::string& value) {
    int length = Parse<int>("length", value);
    if (length < 2) {
      return false;
    }
    length_ = length;
    return true;
  });
}

void ShortTimeMeanScaleNormalization::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size());
}

void ShortTimeMeanScaleNormalization::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
  int back = length_ / 2;
  int front = length_ - back;
  for (size_t i = 0; i < in.Size(); i++) {
    for (int j = 0; j < (int)inputFormat_->Size(); j++) {
      int len = length_;
      int backind = i - back;
      if (backind < 0) {
        len += backind;
        backind = 0;
      }
      int frontind = i + front;
      if (frontind > (int)in.Size()) {
        len += in.Size() - frontind;
        frontind = (int)in.Size();
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

REGISTER_TRANSFORM(ShortTimeMeanScaleNormalization);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
