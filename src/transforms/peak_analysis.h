/*! @file peak_analysis.h
 *  @brief Peaks analysis.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_PEAK_ANALYSIS
#define SRC_TRANSFORMS_PEAK_ANALYSIS

#include <simd/detect_peaks.h>
#include "src/formats/fixed_array.h"
#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class PeakAnalysis
    : public OmpTransformBase<Formats::ArrayFormat<Formats::FixedArray<2>>,
                              Formats::ArrayFormatF> {
 public:
  TRANSFORM_INTRO("PeakAnalysis", "Peak analysis. Calculates the position of "
                                  "the first peak, relative value of the first "
                                  "peak, ratio of the position of the second "
                                  "peak to the positiion of the first peak "
                                  "relative value of the second peak and so "
                                  "on.")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override;

  virtual void Do(const Formats::FixedArray<2>* in, float *out)
      const noexcept override;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_PEAK_ANALYSIS
