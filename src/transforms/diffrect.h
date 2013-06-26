/*! @file diffrect.h
 *  @brief Find the difference from one sample to the next.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_DIFFRECT_H_
#define SRC_TRANSFORMS_DIFFRECT_H_

#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class Diffrect : public OmpUniformFormatTransform<Formats::RawFormatF> {
 public:
  TRANSFORM_INTRO("Diffrect", "Find the difference from one sample to the next.")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual BuffersCountChange OnFormatChanged() override;

  virtual void Do(const float* in, float* out) const noexcept override;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_DIFFRECT_H_
