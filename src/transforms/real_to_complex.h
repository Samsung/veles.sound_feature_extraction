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

#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class RealToComplex
    : public OmpUniformFormatTransform<Formats::ArrayFormatF> {
 public:
  TRANSFORM_INTRO("R2C", "Converts each real number to complex "
                         "number (imaginary part is set to zero).")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in,
                  float* out) const noexcept override;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_REAL_TO_COMPLEX_H_
