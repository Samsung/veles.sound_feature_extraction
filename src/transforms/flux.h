/*! @file flux.h
 *  @brief Measure of spectral change.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_FLUX_H_
#define SRC_TRANSFORMS_FLUX_H_

#include "src/formats/array_format.h"
#include "src/formats/single_format.h"
#include "src/transform_base.h"

namespace sound_feature_extraction {
namespace transforms {

/// @brief \f$||\overrightarrow{Value(f)}-\overrightarrow{Value_pre(f)}||\f$.
class Flux
    : public TransformBase<formats::ArrayFormatF, formats::SingleFormatF> {
 public:
  TRANSFORM_INTRO("Flux", "Measure of spectral change.", Flux)

 protected:
  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float> *out) const noexcept override;

  static float Do(bool simd, const float* input, size_t length,
                  const float* prev) noexcept;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_FLUX_H_
