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

namespace SoundFeatureExtraction {
namespace Transforms {

class SquareRaw
    : public OmpTransformBase<Formats::RawFormat16,
                              Formats::RawFormat32,
                              true> {
 public:
  TRANSFORM_INTRO("Square", "Squares the signal (raw format).")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override;

  virtual void Do(const int16_t* in,
                  int32_t* out) const noexcept override;

  virtual void DoInverse(const int32_t* in,
                         int16_t* out) const noexcept override;

  static void Do(bool simd, const int16_t* input, int length,
                 int32_t* output) noexcept;
};

class SquareF
    : public OmpUniformFormatTransform<Formats::RawFormatF, true> {
 public:
  TRANSFORM_INTRO("Square", "Squares the signal (window floating point "
                            "format).")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual void Do(const float* in,
                  float* out) const noexcept override;

  virtual void DoInverse(const float* in,
                         float* out) const noexcept override;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_SQUARE_H_
