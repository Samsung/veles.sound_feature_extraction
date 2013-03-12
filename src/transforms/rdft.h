/*! @file rft.h
 *  @brief Discrete Fourier Transform on real input using FFT.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_DFT_H_
#define SRC_TRANSFORMS_DFT_H_

#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class RDFT
    : public UniformFormatTransform<Formats::WindowFormatF> {
 public:
  RDFT();

  TRANSFORM_INTRO("RDFT", "Performs Discrete Fourier Transform "
                         "on the input signal (using real FFT).")

  TRANSFORM_PARAMETERS()

  virtual bool HasInverse() const noexcept;

 protected:
  virtual void OnInputFormatChanged();

  virtual void TypeSafeInitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Formats::WindowF>& in,
                          BuffersBase<Formats::WindowF> *out) const noexcept;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_DFT_H_