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

LSP::LSP() : intervals_(kDefaultIntervals), bisects_(kDefaultBisects) {
  RegisterSetter("intervals", [&](const std::string& value) {
    int pv = Parse<int>("intervals", value);
    if (pv < 2) {
      return false;
    }
    intervals_ = pv;
    return true;
  });
  RegisterSetter("bisects", [&](const std::string& value) {
    int pv = Parse<int>("bisects", value);
    if (pv < 1) {
      return false;
    }
    bisects_ = pv;
    return true;
  });
}

void LSP::Do(const float* in, float* out) const noexcept {
  lpc_to_lsp(UseSimd(), in, input_format_->Size(), bisects_,
             2.f / intervals_, out);
}

REGISTER_TRANSFORM(LSP);

}  // namespace transforms
}  // namespace sound_feature_extraction
