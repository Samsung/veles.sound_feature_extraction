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
#include <mutex>
#include <vector>

namespace SoundFeatureExtraction {
namespace Transforms {

class Diff : public OmpUniformFormatTransform<Formats::RawFormatF> {
 public:
  Diff();

  TRANSFORM_INTRO("Diff", "Differentiate the signal.")

  OMP_TRANSFORM_PARAMETERS(
      TP("rectify", "Perform half-wave rectification (retain only if the "
                    "difference is positive).",
         "false")
      TP("swt", "Smoothly differentiate using Stationary Wavelet Transform "
                "(db1) of the specified level. The level must be greater than "
                "or equal to 0. If set to zero, this parameter is ignored.",
         std::to_string(kNoSWT))
  )

 protected:
  struct SWTBuffers {
    SWTBuffers() : first(nullptr, std::free), second (nullptr, std::free) {
    }

    SWTBuffers(const SWTBuffers&)
        : first(nullptr, std::free), second (nullptr, std::free) {
    }

    FloatPtr first;
    FloatPtr second;
    std::mutex mutex;
  };

  virtual void Initialize() const noexcept override;

  virtual void Do(const float* in, float* out) const noexcept override;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;

  static void DoRectify(bool simd, const float* input, int length,
                        float* output) noexcept;

  static void Rectify(bool simd, const float* input, int length,
                      float* output) noexcept;

  static void DifferentiateUsingSWT(int level, const float* input, int length,
                                    SWTBuffers* auxBuffers,
                                    float* output) noexcept;

  static constexpr int kNoSWT = 0;

 private:
  bool rectify_;
  int swt_;
  mutable std::vector<SWTBuffers> swt_buffers_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_DIFF_H_
