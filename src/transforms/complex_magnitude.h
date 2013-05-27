/*! @file complex_magnitude.h
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

#ifndef SRC_TRANSFORMS_COMPLEX_MAGNITUDE_H_
#define SRC_TRANSFORMS_COMPLEX_MAGNITUDE_H_

#include "src/formats/window_format.h"
#include "src/omp_transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class ComplexMagnitude
    : public OmpUniformFormatTransform<Formats::WindowFormatF>,
      public TransformLogger<ComplexMagnitude> {
 public:
  TRANSFORM_INTRO("ComplexMagnitude",
                  "Calculates the magnitude of each complex number, that is, "
                  " a square root of the sum of squared real and imaginary "
                  "parts.")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual void OnFormatChanged();

  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers)
  const noexcept;

  virtual void Do(const Formats::WindowF& in,
                  Formats::WindowF* out) const noexcept;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_COMPLEX_MAGNITUDE_H_
