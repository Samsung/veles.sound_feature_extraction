/*! @file lpc_cc.cc
 *  @brief Convert Linear Prediction Coefficients (LPC2CC) to cepstral format.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/lpc_cc.h"
#include <cmath>

namespace sound_feature_extraction {
namespace transforms {

RTP(LPC2CC, size)

LPC2CC::LPC2CC() : size_(kDefaultSize) {
}

bool LPC2CC::validate_size(const int& value) noexcept {
   return value >= 2;
}

size_t LPC2CC::OnFormatChanged(size_t buffersCount) {
  if (size_ == kDefaultSize) {
    output_format_->SetSize(input_format_->Size());
  } else {
    output_format_->SetSize(size_);
  }
  return buffersCount;
}

void LPC2CC::Do(const float* in, float* out) const noexcept {
  out[0] = logf(in[0]);
  int in_size = input_format_->Size();
  int size = size_ == 0? (in_size + 1) : size_;
  for (int m = 1; m < size; m++) {
    float cm = 0;
    for (int k = 1; k < std::min(m, in_size); k++) {
      cm -= (m - k) * in[k] * out[m - k];
    }
    cm /= m;
    if (m < in_size) {
      cm -= in[m];
    }
    out[m] = cm;
  }
}

REGISTER_TRANSFORM(LPC2CC);

}  // namespace transforms
}  // namespace sound_feature_extraction
