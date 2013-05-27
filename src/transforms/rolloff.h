/*! @file rolloff.h
 *  @brief Measure of spectral shape.
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
#include "src/omp_transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

/// @brief \f$R: \displaystyle\sum_{f=0}^{f=R}{Value[f]} = ratio \times \displaystyle\sum_{f=0}^{f=N}{Value[f]}\f$.
class Rolloff
    : public OmpTransformBase<Formats::WindowFormatF, Formats::SingleFormatF> {
 public:
  Rolloff();

  TRANSFORM_INTRO("Rolloff", "Measure of spectral shape.")

  OMP_TRANSFORM_PARAMETERS(
      TP("ratio", "The ratio between the partial sum and the whole sum.",
         std::to_string(kDefaultRatio))
  )

 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<float>* buffers) const noexcept;

  virtual void Do(const Formats::WindowF& in,
                  float* out) const noexcept;

  static int Do(bool simd, const float* input, size_t length,
                float ratio) noexcept;

 private:
  static const float kDefaultRatio;
  float ratio_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_ROLLOFF_H_
