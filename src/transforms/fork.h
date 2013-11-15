/*! @file fork.h
 *  @brief Window cloning.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_FORK_H_
#define SRC_TRANSFORMS_FORK_H_

#include "src/formats/array_format.h"
#include "src/transform_base.h"

namespace sound_feature_extraction {
namespace transforms {

class Fork : public UniformFormatTransform<formats::ArrayFormatF> {
  friend class FrequencyBands;
 public:
  Fork();

  TRANSFORM_INTRO("Fork", "Clones the windows, increasing their number by "
                          "a factor of \"factor\".",
                  Fork)

  TP(factor, int, kDefaultFactor, "Windows number multiplier value.")

 protected:
  static constexpr int kDefaultFactor = 4;

  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*>* out) const noexcept override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_FORK_H_
