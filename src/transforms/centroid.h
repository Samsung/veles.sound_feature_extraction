/*! @file centroid.h
 *  @brief Centroid (window's center of mass in frequency domain) calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_CENTROID_H_
#define SRC_TRANSFORMS_CENTROID_H_

#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

/// @brief Calculates \f$\frac{\displaystyle\sum_{f=0}^{SamplingRate / 2}{f
/// Value[f]}}{\displaystyle\sum_{f=0}^{SamplingRate / 2}{Value[f]}}\f$.
class Centroid
    : public OmpTransformBase<Formats::RawFormatF, Formats::SingleFormatF> {
 public:
  TRANSFORM_INTRO("Centroid", "Window's center of mass in frequency domain "
                              "calculation.")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual void Do(const float* in,
                  float* out) const noexcept override;

  static float Do(bool simd, const float* input, size_t length) noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_CENTROID_H_
