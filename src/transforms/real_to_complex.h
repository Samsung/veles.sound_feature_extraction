/*! @file real_to_complex.h
 *  @brief Converts real numbers to the corresponding complex numbers.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_REAL_TO_COMPLEX_H_
#define SRC_TRANSFORMS_REAL_TO_COMPLEX_H_

#include <string>
#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class RealToComplex
    : public UniformFormatTransform<Formats::WindowFormatF> {
 public:
  RealToComplex();

  TRANSFORM_INTRO("R2C", "Converts each real number to complex "
                         "number (imaginary part is set to zero).")

  TRANSFORM_PARAMETERS()

 protected:
  virtual void OnFormatChanged();

  virtual void TypeSafeInitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Formats::WindowF>& in,
                          BuffersBase<Formats::WindowF> *out) const noexcept;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_REAL_TO_COMPLEX_H_
