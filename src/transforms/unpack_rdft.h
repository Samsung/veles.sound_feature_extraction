/*! @file unpack_rdft.h
 *  @brief Unpacks data after applying RDFT.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_UNPACK_RDFT_H_
#define SRC_TRANSFORMS_UNPACK_RDFT_H_

#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class UnpackRDFT
    : public OmpUniformFormatTransform<Formats::RawFormatF> {
 public:
  TRANSFORM_INTRO("UnpackRDFT", "Unpacks the result after applying RDFT.")

  OMP_TRANSFORM_PARAMETERS()

  void Initialize() const noexcept override;

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in,
                  float* out) const noexcept override;

 private:
  mutable bool realMode_;
  mutable size_t length_;
  mutable size_t offset_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_UNPACK_RDFT_H_
