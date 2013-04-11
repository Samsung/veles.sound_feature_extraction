/*! @file complex_to_real.h
 *  @brief Converts complex numbers to the corresponding real numbers.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_COMPLEX_TO_REAL_H_
#define SRC_TRANSFORMS_COMPLEX_TO_REAL_H_

#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class ComplexToReal
    : public UniformFormatTransform<Formats::WindowFormatF> {
 public:
  TRANSFORM_INTRO("C2R", "Converts each complex number to corresponding "
                         "real numbers.")

  TRANSFORM_PARAMETERS()

 protected:
  virtual void OnFormatChanged();

  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                  BuffersBase<Formats::WindowF>* out) const noexcept;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_COMPLEX_TO_REAL_H_
