/*! @file diff.h
 *  @brief Find the difference from one sample to the next.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_DIFF_H_
#define SRC_TRANSFORMS_DIFF_H_

#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class Diff : public OmpUniformFormatTransform<Formats::RawFormatF> {
 public:
  Diff();

  TRANSFORM_INTRO("Diff", "Find the difference from one sample to the next.")

  OMP_TRANSFORM_PARAMETERS(
      TP("rectify", "Perform half-wave rectification (retain only if the "
                    "difference is positive).",
         "false"))

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in, float* out) const noexcept override;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;

  static void DoRectify(bool simd, const float* input, int length,
                        float* output) noexcept;

 private:
  bool rectify_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_DIFF_H_
