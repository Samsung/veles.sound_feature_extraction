/*! @file dft.h
 *  @brief Discrete Fourier Transform using FFT.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef TEMPLATE_TRANSFORM_H_
#define TEMPLATE_TRANSFORM_H_

#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class DFT
    : public UniformFormatTransform<Formats::WindowFormatF> {
 public:
  DFT();

  TRANSFORM_INTRO("DFT", "Performs Discrete Fourier Transform "
                         "on the signal (using real FFT).")

  TRANSFORM_PARAMETERS()

  virtual bool HasInverse() const noexcept;

 protected:
  virtual void TypeSafeInitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Formats::WindowF>& in,
                          BuffersBase<Formats::WindowF> *out) const noexcept;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_TEMPLATE_TRANSFORM_H_
