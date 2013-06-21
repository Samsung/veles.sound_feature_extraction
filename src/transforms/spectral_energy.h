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

#include "src/formats/window_format.h"
#include "src/omp_transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class SpectralEnergy
    : public OmpUniformFormatTransform<Formats::WindowFormatF>,
      public TransformLogger<SpectralEnergy> {
 public:
  TRANSFORM_INTRO("SpectralEnergy",
                  "Calculates the squared magnitude of each complex number, "
                  "that is, the sum of squared real and imaginary parts.")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual void OnFormatChanged() override;

  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers)
  const noexcept;

  virtual void Do(const Formats::WindowF& in,
                  Formats::WindowF* out) const noexcept override;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_SPECTRAL_ENERGY_H_
