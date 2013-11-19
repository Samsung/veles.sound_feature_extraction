/*! @file peak_dynamic_programming.h
 *  @brief Accurate average estimation based on dynamic programming.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_PEAK_DYNAMIC_PROGRAMMING_H_
#define SRC_TRANSFORMS_PEAK_DYNAMIC_PROGRAMMING_H_

#include "src/formats/fixed_array.h"
#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class PeakDynamicProgramming : public TransformBase<
    formats::ArrayFormat<formats::FixedArray<2>>, formats::SingleFormatF> {
 public:
  PeakDynamicProgramming();

  TRANSFORM_INTRO("PeakDynamicProgramming",
                  "Accurate average estimation using dynamic programming "
                  "approach.",
                  PeakDynamicProgramming)

  TP(mind_values, bool, kDefaultMindValues,
     "Whether to favor points with higher values.")

 protected:
  virtual void Do(const BuffersBase<formats::FixedArray<2>*>& in,
                  BuffersBase<float>* out)
      const noexcept override;

  static constexpr bool kDefaultMindValues = false;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_PEAK_DYNAMIC_PROGRAMMING_H_
