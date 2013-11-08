/*! @file lpc.h
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

#ifndef SRC_TRANSFORMS_LPC_H_
#define SRC_TRANSFORMS_LPC_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class LPC : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  TRANSFORM_INTRO("LPC", "Calculate Linear Prediction Coefficients (LPC) using "
                         "Levinson-Durbin recursion from autocorrelation "
                         "coefficients.")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in, float* out) const noexcept override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_LPC_H_
