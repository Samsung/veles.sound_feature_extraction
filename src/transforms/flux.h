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

#ifndef SRC_TRANSFORMS_ROLLOFF_H_
#define SRC_TRANSFORMS_ROLLOFF_H_

#include "src/formats/window_format.h"
#include "src/formats/single_format.h"
#include "src/transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

/// @brief \f$||\overrightarrow{Value(f)}-\overrightarrow{Value_pre(f)}||\f$.
class Flux
    : public TransformBase<Formats::WindowFormatF, Formats::SingleFormatF> {
 public:
  TRANSFORM_INTRO("Flux", "Measure of spectral change.")

  TRANSFORM_PARAMETERS()

 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<float>* buffers) const noexcept override;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                  BuffersBase<float> *out) const noexcept override;

  static float Do(bool simd, const float* input, size_t length,
                  const float* prev) noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_ROLLOFF_H_
