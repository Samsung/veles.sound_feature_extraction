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

namespace SoundFeatureExtraction {
namespace Transforms {

class Preemphasis
    : public OmpUniformFormatTransform<Formats::RawFormat16> {
 public:
  Preemphasis();

  TRANSFORM_INTRO("Preemphasis", "Filter the signal with a first-order "
                                 "high-pass filter y[n] = 1 - k * x[n - 1].")

  OMP_TRANSFORM_PARAMETERS(
      TP("value", "The filter coefficient from range (0..1]. "
                  "The higher, the more emphasis occurs.",
         std::to_string(kDefaultK))
  )

 protected:
  static const float kDefaultK;

  virtual void Do(const int16_t* in,
                  int16_t* out) const noexcept override;

  static void Do(bool simd, const int16_t* input, size_t length,
                 float k, int16_t* output) noexcept;

 private:
  float k_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_PREEMPHASIS_H_
