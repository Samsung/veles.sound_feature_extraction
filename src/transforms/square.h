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

#include "src/formats/raw_format.h"
#include "src/formats/window_format.h"
#include "src/omp_transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class SquareRaw
    : public TransformBase<Formats::RawFormat16, Formats::RawFormat32, true> {
 public:
  TRANSFORM_INTRO("Square", "Squares the signal (raw format).")

  TRANSFORM_PARAMETERS()

 protected:
  virtual void OnInputFormatChanged();

  virtual void InitializeBuffers(
      const BuffersBase<Formats::Raw16>& in,
      BuffersBase<Formats::Raw32>* buffers) const noexcept;

  virtual void InitializeBuffers(
      const BuffersBase<Formats::Raw32>& in,
      BuffersBase<Formats::Raw16>* buffers) const noexcept;

  virtual void Do(const BuffersBase<Formats::Raw16>& in,
                  BuffersBase<Formats::Raw32>* out) const noexcept;

  virtual void Do(const BuffersBase<Formats::Raw32>& in,
                  BuffersBase<Formats::Raw16>* out) const noexcept;

  static void Do(bool simd, const int16_t* input, int length,
                 int32_t* output) noexcept;
};

class SquareWindow
    : public OmpUniformFormatTransform<Formats::WindowFormatF, true> {
 public:
  TRANSFORM_INTRO("Square", "Squares the signal (window floating point "
                            "format).")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void Do(const Formats::WindowF& in,
                  Formats::WindowF* out) const noexcept;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_SQUARE_H_
