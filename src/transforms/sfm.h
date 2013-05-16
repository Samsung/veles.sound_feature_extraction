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

namespace SoundFeatureExtraction {
namespace Transforms {

class SFM
    : public TransformBase<
          Formats::SingleFormat<Formats::FixedArray<MEAN_TYPE_COUNT>>,
          Formats::SingleFormat<float>>,
      public TransformLogger<SFM> {
 public:
  TRANSFORM_INTRO("SFM", "Spectral Flatness Measure calculation.")

  TRANSFORM_PARAMETERS()

 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::FixedArray<MEAN_TYPE_COUNT>>& in,
      BuffersBase<float>* buffers)
  const noexcept;

  virtual void Do(const BuffersBase<Formats::FixedArray<MEAN_TYPE_COUNT>>& in,
                  BuffersBase<float> *out) const noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_SFM_H_
