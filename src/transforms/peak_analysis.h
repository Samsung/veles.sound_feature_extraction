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

#include "src/formats/fixed_array.h"
#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class PeakAnalysis : public OmpTransformBase<
    formats::ArrayFormat<formats::FixedArray<2>>,
    formats::ArrayFormatF>  {
 public:
  TRANSFORM_INTRO("PeakAnalysis", "Peak analysis. This is a modified version "
                                  "of the algorithm from \"Automatic Musical "
                                  "Genre Classification Of Audio Signals\".",
                  PeakAnalysis)

 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override;

  virtual void Do(const formats::FixedArray<2>* in, float* out)
      const noexcept override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_PEAK_ANALYSIS
