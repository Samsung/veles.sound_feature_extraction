/*! @file lpc.cc
 *  @brief Find Linear Prediction Coefficients (LPC) using Levinson-Durbin
 *  recursion.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/lpc.h"
#include "src/primitives/lpc.h"

namespace sound_feature_extraction {
namespace transforms {

LPC::LPC() : error_(false) {
  RegisterSetter("add_error", [&](const std::string& value) {
    error_ = Parse<bool>("add_error", value);
    return true;
  });
}

size_t LPC::OnFormatChanged(size_t buffersCount) {
  if (!error_) {
    output_format_->SetSize(input_format_->Size() - 1);
  } else {
    output_format_->SetSize(input_format_->Size());
  }
  return buffersCount;
}

void LPC::Do(const float* in, float* out) const noexcept {
  float err = ldr_lpc(UseSimd(), in, input_format_->Size(),
                      out + (error_? 1 : 0));
  if (error_) {
    out[0] = err;
  }
}

REGISTER_TRANSFORM(LPC);

}  // namespace transforms
}  // namespace sound_feature_extraction
