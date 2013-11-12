/*! @file lpc_cc.h
 *  @brief Convert Linear Prediction Coefficients (LPC) to cepstral format.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_LPC_CC_H_
#define SRC_TRANSFORMS_LPC_CC_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class LPC2CC : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  LPC2CC();

  TRANSFORM_INTRO("LPCtoCC", "Convert Linear Prediction Coefficients (LPC) to "
                             "Cepstral Coefficients.")

  OMP_TRANSFORM_PARAMETERS(
      TP("size", "The number of cepstral coefficients. 0 means original LPC "
                 "length.",
         std::to_string(kDefaultSize))
  )

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in, float* out) const noexcept override;

 private:
  static constexpr int kDefaultSize = 0;

  int size_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_LPC_CC_H_
