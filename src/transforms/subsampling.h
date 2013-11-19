/*! @file subsampling.h
 *  @brief Subsample the signal by an integer factor.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_SUBSAMPLING_H_
#define SRC_TRANSFORMS_SUBSAMPLING_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class Subsampling : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  Subsampling();

  TRANSFORM_INTRO("Subsample",
                  "Subsample the signal by an integer factor.",
                  Subsampling)

  TP(factor, int, kDefaultFactor,
     "Calculate normalized subsampling by dividing each "
     "value by lag 0 result (squared signal sum).")

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in, float* out) const noexcept override;

  static constexpr int kDefaultFactor = 1;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_SUBSAMPLING_H_
