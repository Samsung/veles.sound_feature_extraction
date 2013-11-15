/*! @file lsp.cc
 *  @brief Converts Linear Prediction Coefficients (LPC) to more robust
 *  Line Spectral Pairs (LSP).
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/lsp.h"
#include "src/primitives/lsp.h"

namespace sound_feature_extraction {
namespace transforms {

RTP(LSP, intervals)
RTP(LSP, bisects)

LSP::LSP() : intervals_(kDefaultIntervals), bisects_(kDefaultBisects) {
}

bool LSP::validate_intervals(const int& value) noexcept {
  return value >= 2;
}

bool LSP::validate_bisects(const int& value) noexcept {
  return value > 1;
}

void LSP::Do(const float* in, float* out) const noexcept {
  lpc_to_lsp(use_simd(), in, input_format_->Size(), bisects_,
             2.f / intervals_, out);
}

REGISTER_TRANSFORM(LSP);

}  // namespace transforms
}  // namespace sound_feature_extraction
