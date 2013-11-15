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

namespace sound_feature_extraction {
namespace transforms {

class RDFT : public UniformFormatTransform<formats::ArrayFormatF> {
 public:
  TRANSFORM_INTRO("RDFT", "Performs Discrete Fourier Transform "
                          "on the input signal (using real FFT).",
                  RDFT)

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*>* out) const noexcept override;
};

class RDFTInverse
    : public InverseUniformFormatTransform<RDFT> {
 public:
  virtual bool BufferInvariant() const noexcept override final {
    return true;
  }

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*>* out) const noexcept override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_RDFT_H_
