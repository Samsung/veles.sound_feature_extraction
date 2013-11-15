/*! @file sfm.h
 *  @brief Arithmetic and geometric sfms calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_SFM_H_
#define SRC_TRANSFORMS_SFM_H_

#include "src/transforms/mean.h"

namespace sound_feature_extraction {
namespace transforms {

class SFM : public OmpTransformBase<
    formats::SingleFormat<formats::FixedArray<kMeanTypeCount>>,
    formats::SingleFormat<float>>, public TransformLogger<SFM> {
 public:
  TRANSFORM_INTRO("SFM", "Spectral Flatness Measure calculation.", SFM)

 protected:
  virtual void Do(const formats::FixedArray<kMeanTypeCount>& in,
                  float* out) const noexcept override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_SFM_H_
