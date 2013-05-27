/*! @file zerocrossings.h
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

#ifndef SRC_TRANSFORMS_ZEROCROSSINGS_H_
#define SRC_TRANSFORMS_ZEROCROSSINGS_H_

#include "src/formats/raw_format.h"
#include "src/formats/single_format.h"
#include "src/formats/window_format.h"
#include "src/omp_transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class ZeroCrossingsWindow
    : public OmpTransformBase<Formats::WindowFormatF, Formats::SingleFormat32> {
 public:
  TRANSFORM_INTRO("ZeroCrossings", "Number of time domain zero crossings "
                                   "of the signal.")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<int32_t>* buffers) const noexcept;

  virtual void Do(const Formats::WindowF& in,
                  int32_t* out) const noexcept;

  static int Do(bool simd, const float* input, size_t length) noexcept;
};

class ZeroCrossingsRaw
    : public TransformBase<Formats::RawFormat16, Formats::SingleFormat32> {
 public:
  TRANSFORM_INTRO("ZeroCrossings", "Number of time domain zero crossings "
                                   "of the signal.")

  TRANSFORM_PARAMETERS()

 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::Raw16>& in,
      BuffersBase<int32_t>* buffers) const noexcept;

  virtual void Do(const BuffersBase<Formats::Raw16>& in,
                  BuffersBase<int32_t> *out) const noexcept;

  static int Do(bool simd, const int16_t* input, size_t length) noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_ZEROCROSSINGS_H_
