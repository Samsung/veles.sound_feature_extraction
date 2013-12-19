/*! @file stretch.h
 *  @brief Stretch the signal by duplicating its values.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_STRETCH_H_
#define SRC_TRANSFORMS_STRETCH_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class Stretch : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  Stretch();

  TRANSFORM_INTRO("Stretch", "Stretch the signal by an integer factor.",
                  Stretch)

  TP(factor, float, kDefaultFactor,
     "Stretch the signal by duplicating its values.")
  TP(center, bool, kDefaultCenter,
     "Center the intervals, extending the corners (useful for stretching after "
     "splitting into windows.).")

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in, float* out) const noexcept override;

  static constexpr float kDefaultFactor = 1;
  static constexpr bool kDefaultCenter = false;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_STRETCH_H_
