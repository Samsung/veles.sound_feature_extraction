/*! @file stretch.cc
 *  @brief Stretch the signal by duplicating its values.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/stretch.h"
#include <simd/memory.h>

namespace sound_feature_extraction {
namespace transforms {

Stretch::Stretch() : factor_(kDefaultFactor) {
}

bool Stretch::validate_factor(const int& value) noexcept {
  return value >= 1;
}

size_t Stretch::OnFormatChanged(size_t buffersCount) {
  output_format_->SetSize(input_format_->Size() * factor_);
  return buffersCount;
}

void Stretch::Do(const float* in, float* out) const noexcept {
  if (factor_ == 1) {
    memcpy(out, in, input_format_->SizeInBytes());
    return;
  }
  if (factor_ < 4) {
    for (int i = 0; i < static_cast<int>(output_format_->Size()); i++) {
      out[i] = in[i / factor_];
    }
    return;
  }
  for (int i = 0; i < static_cast<int>(output_format_->Size()); i += factor_) {
    memsetf(out + i, in[i / factor_], factor_);
  }
}

RTP(Stretch, factor)
REGISTER_TRANSFORM(Stretch);

}  // namespace transforms
}  // namespace sound_feature_extraction
