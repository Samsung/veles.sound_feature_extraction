/*! @file hps.h
 *  @brief Extracting fundamental frequency with Harmonic Product Spectrum algorithm
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_HPS_H_
#define SRC_TRANSFORMS_HPS_H_

#include <string>
#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

/// @brief Harmonic Product Spectrum calculation.
class Hps
    : public OmpTransformBase<Formats::ArrayFormatF, Formats::SingleFormatF>,
      public TransformLogger<Hps> {
 public:
  Hps();

  TRANSFORM_INTRO("HPS", "Extracting fundamental frequency with Harmonic "
                         "Product Spectrum algorithm.")

  OMP_TRANSFORM_PARAMETERS(
      TP("length", "The number of harmonics to multiply.",
         std::to_string(kDefaultCoefficientsNumber))
  )

 protected:
  virtual void Do(const float* in, float* out) const noexcept override;

 private:
  static constexpr int kDefaultCoefficientsNumber = 3;

  int length_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_HPS_H_
