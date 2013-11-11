/*! @file dct.h
 *  @brief Discrete Cosine Transform calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_DCT_H_
#define SRC_TRANSFORMS_DCT_H_

#include "src/formats/array_format.h"
#include "src/transform_base.h"

namespace sound_feature_extraction {
namespace transforms {

class DCT : public UniformFormatTransform<formats::ArrayFormatF> {
 public:
  TRANSFORM_INTRO("DCT", "Performs Discrete Cosine Transform "
                         "on the signal.")

  TRANSFORM_PARAMETERS()

 protected:
  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*>* out) const noexcept override;
};

class DCTInverse : public InverseUniformFormatTransform<DCT> {
 public:
  TRANSFORM_PARAMETERS()

  virtual bool BufferInvariant() const noexcept override final {
    return true;
  }

 protected:
  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*>* out) const noexcept override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_DCT_H_
