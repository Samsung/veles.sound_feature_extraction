/*! @file preemphasis.h
 *  @brief Number of time domain zero crossings of the signal.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_PREEMPHASIS_H_
#define SRC_TRANSFORMS_PREEMPHASIS_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class Preemphasis : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  Preemphasis();

  TRANSFORM_INTRO("Preemphasis", "Filter the signal with a first-order "
                                 "high-pass filter y[n] = 1 - k * x[n - 1].",
                  Preemphasis)

  TP(value, float, kDefaultValue,
     "The filter coefficient from range (0..1]. "
     "The higher, the more emphasis occurs.")

 protected:
  static constexpr float kDefaultValue = 0.9f;

  virtual void Do(const float* in, float* out) const noexcept override;

  static void Do(bool simd, const float* input, size_t length,
                 float k, float* output) noexcept;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_PREEMPHASIS_H_
