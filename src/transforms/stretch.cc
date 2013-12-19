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
#include <cmath>
#include <simd/memory.h>

namespace sound_feature_extraction {
namespace transforms {

Stretch::Stretch() : factor_(kDefaultFactor), center_(kDefaultCenter) {
}

bool Stretch::validate_factor(const float& value) noexcept {
  return value >= 1;
}

ALWAYS_VALID_TP(Stretch, center)

size_t Stretch::OnFormatChanged(size_t buffersCount) {
  output_format_->SetSize(input_format_->Size() * factor_);
  return buffersCount;
}

void Stretch::Do(const float* in, float* out) const noexcept {
  if (factor_ == 1) {
    memcpy(out, in, input_format_->SizeInBytes());
    return;
  }
  float ilen = factor_ * input_format_->Size() / (input_format_->Size() + 1);
  if (!center_ || floorf(factor_ - ilen) == 0 || factor_ < 4) {
    if (factor_ < 4) {
      for (int i = 0; i < static_cast<int>(output_format_->Size()); i++) {
        out[i] = in[static_cast<int>(floorf(i / factor_))];
      }
      return;
    }
    for (int i = 0; i < static_cast<int>(input_format_->Size()); i++) {
      memsetf(out + static_cast<int>(floorf(i * factor_)), in[i], factor_);
    }
    return;
  }
  // Slow but simple version
  #if 0
    for (int i = ilen / 2;
         i < static_cast<int>(output_format_->Size()) - ilen / 2;
         i++) {
      out[i] = in[static_cast<int>(floorf((i - ilen / 2) / ilen))];
    }
    for (int i = 0; i < ilen / 2; i++) {
      out[i] = in[0];
    }
    for (int i = static_cast<int>(output_format_->Size()) - ilen / 2;
         i < static_cast<int>(output_format_->Size());
         i++) {
      out[i] = in[input_format_->Size() - 1];
    }
    return;
  #endif
  for (int i = 0; i < static_cast<int>(input_format_->Size()); i++) {
    memsetf(out + static_cast<int>(floorf(i * ilen + ilen / 2)),
            in[i], static_cast<int>(floorf((i + 1) * ilen - i * ilen)));
  }
  memsetf(out, in[0], ilen / 2);
  int last_index = input_format_->Size() * ilen + ilen / 2;
  memsetf(out + last_index, in[input_format_->Size() -1],
          output_format_->Size() - last_index);
}

RTP(Stretch, factor)
RTP(Stretch, center)
REGISTER_TRANSFORM(Stretch);

}  // namespace transforms
}  // namespace sound_feature_extraction
