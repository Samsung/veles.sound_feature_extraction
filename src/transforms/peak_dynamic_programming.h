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

#ifndef SRC_TRANSFORMS_PEAK_DYNAMIC_PROGRAMMING
#define SRC_TRANSFORMS_PEAK_DYNAMIC_PROGRAMMING

#include "src/formats/fixed_array.h"
#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class PeakDynamicProgramming
    : public TransformBase<Formats::ArrayFormat<Formats::FixedArray<2>>,
                           Formats::SingleFormatF> {
 public:
  PeakDynamicProgramming();

  TRANSFORM_INTRO("PeakDynamicProgramming",
                  "Accurate average estimation using dynamic programming "
                  "approach.")

  TRANSFORM_PARAMETERS(
      TP("mind_values", "Whether to favor points with higher values.",
         std::to_string(kDefaultMindValues))
  )

 protected:
  virtual void Do(const BuffersBase<Formats::FixedArray<2>*>& in,
                  BuffersBase<float>* out)
      const noexcept override;

 private:
  static constexpr bool kDefaultMindValues = false;

  bool mind_values_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_PEAK_DYNAMIC_PROGRAMMING
