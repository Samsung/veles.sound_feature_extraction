/*! @file rectify.h
 *  @brief Wave rectification.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_RECTIFY_H_
#define SRC_TRANSFORMS_RECTIFY_H_

#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class Rectify : public OmpUniformFormatTransform<Formats::RawFormatF> {
 public:
  TRANSFORM_INTRO("Rectify", "Wave rectification to decrease high-frequency "
                             "content.")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual void Do(const float* in, float* out) const noexcept override;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_RECTIFY_H_
