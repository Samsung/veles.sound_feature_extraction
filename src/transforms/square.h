/*! @file square.h
 *  @brief Square the input signal.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_SQUARE_H_
#define SRC_TRANSFORMS_SQUARE_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class Square : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  TRANSFORM_INTRO("Square", "Squares the signal (window floating point "
                            "format).")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual void Do(const float* in, float* out) const noexcept override;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;
};

class SquareInverse : public OmpInverseUniformFormatTransform<Square> {
 public:
  OMP_TRANSFORM_PARAMETERS()

  virtual void Do(const float* in, float* out) const noexcept override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_SQUARE_H_
