/*! @file beat_dynamic_programming.h
 *  @brief Accurate tempo estimation based on dynamic programming.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_BEAT_DYNAMIC_PROGRAMMING
#define SRC_TRANSFORMS_BEAT_DYNAMIC_PROGRAMMING

#include "src/formats/fixed_array.h"
#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class BeatDynamicProgramming
    : public TransformBase<Formats::ArrayFormat<Formats::FixedArray<2>>,
                           Formats::SingleFormatF> {
 public:
  BeatDynamicProgramming();

  TRANSFORM_INTRO("BeatDynamicProgramming",
                  "Accurate tempo estimation using dynamic programming "
                  "approach.")

  TRANSFORM_PARAMETERS(
      TP("mind_values", "Whether to favor points with higher values.",
         "true")
  )

 protected:
  virtual void Do(const BuffersBase<Formats::FixedArray<2>*>& in,
                  BuffersBase<float>* out)
      const noexcept override;

 private:
  bool mind_values_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_BEAT_DYNAMIC_PROGRAMMING
