/*! @file rdft.h
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

#ifndef SRC_TRANSFORMS_RDFT_H_
#define SRC_TRANSFORMS_RDFT_H_

#include "src/formats/array_format.h"
#include "src/transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class RDFT : public UniformFormatTransform<Formats::ArrayFormatF> {
 public:
  TRANSFORM_INTRO("RDFT", "Performs Discrete Fourier Transform "
                          "on the input signal (using real FFT).")

  TRANSFORM_PARAMETERS()

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*>* out) const noexcept override;
};

class RDFTInverse
    : public InverseUniformFormatTransform<RDFT> {
 public:
  TRANSFORM_PARAMETERS()

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*>* out) const noexcept override;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_RDFT_H_
