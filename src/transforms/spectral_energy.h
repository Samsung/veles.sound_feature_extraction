/*! @file spectral_energy.h
 *  @brief Calculate the magnitude of each complex number.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_SPECTRAL_ENERGY_H_
#define SRC_TRANSFORMS_SPECTRAL_ENERGY_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class SpectralEnergy
    : public OmpUniformFormatTransform<formats::ArrayFormatF>,
      public TransformLogger<SpectralEnergy> {
 public:
  TRANSFORM_INTRO("SpectralEnergy",
                  "Calculates the squared magnitude of each complex number, "
                  "that is, the sum of squared real and imaginary parts.")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in,
                  float* out) const noexcept override;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_SPECTRAL_ENERGY_H_
