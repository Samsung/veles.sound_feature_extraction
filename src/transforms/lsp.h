/*! @file lsp.h
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

#ifndef SRC_TRANSFORMS_LSP_H_
#define SRC_TRANSFORMS_LSP_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class LSP : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  LSP();

  TRANSFORM_INTRO("LSP", "Converts Linear Prediction Coefficients (LPC) to "
                         "more robust Line Spectral Pairs (LSP) "
                         "representation.")

  OMP_TRANSFORM_PARAMETERS(
      TP("intervals", "The number of intervals to scan in the range [-1, 1]. "
                      "The corresponding delta equals to 2 / intervals. The "
                      "bigger the number, the less probability is to skip "
                      "a root.",
         std::to_string(kDefaultIntervals))
      TP("bisects", "The number of bisections for the root value refinement. "
                    "The bigger the number, the more precise the values are "
                    "and the less probability is to skip "
                    "a root.",
         std::to_string(kDefaultBisects))
  )

 protected:
  virtual void Do(const float* in, float* out) const noexcept override;

 private:
  static constexpr int kDefaultIntervals = 128;
  static constexpr int kDefaultBisects = 16;

  int intervals_;
  int bisects_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_LSP_H_
