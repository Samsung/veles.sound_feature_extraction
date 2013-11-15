/*! @file rolloff.h
 *  @brief Measure of spectral shape.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_ROLLOFF_H_
#define SRC_TRANSFORMS_ROLLOFF_H_

#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

/// @brief \f$R: \displaystyle\sum_{f=0}^{f=R}{Value[f]} = ratio \times \displaystyle\sum_{f=0}^{f=N}{Value[f]}\f$.
class Rolloff : public OmpTransformBase<formats::ArrayFormatF,
                                        formats::SingleFormatF>  {
 public:
  Rolloff();

  TRANSFORM_INTRO("Rolloff", "Measure of spectral shape.", Rolloff)

  TP(ratio, float, kDefaultRatio,
     "The ratio between the partial sum and the whole sum.")

 protected:
  virtual void Do(const float* in, float* out) const noexcept override;

  static int Do(bool simd, const float* input, size_t length,
                float ratio) noexcept;

 private:
  static constexpr float kDefaultRatio = 0.85f;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_ROLLOFF_H_
