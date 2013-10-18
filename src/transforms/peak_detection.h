/*! @file peak_detection.h
 *  @brief Peak information extraction.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_PEAK_DETECTION
#define SRC_TRANSFORMS_PEAK_DETECTION

#include <simd/detect_peaks.h>
#include "src/formats/fixed_array.h"
#include "src/transforms/common.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class PeakDetection
    : public OmpTransformBase<Formats::ArrayFormatF,
                              Formats::ArrayFormat<Formats::FixedArray<2>>> {
 public:
  PeakDetection();

  TRANSFORM_INTRO("Peaks", "Peak detection, that is, extraction of maximums "
                  "and/or minimums from the signal.")

  OMP_TRANSFORM_PARAMETERS(
      TP("sort", "The order in which to sort peaks (position or value).",
         "value")
      TP("number", "The maximal number of peaks to extract. The rest of the "
                   "slots will be set to zero.",
         std::to_string(kDefaultPeaksNumber))
      TP("type", "The type of the peaks: max, min or all.", "max")
  )

 protected:
  typedef enum {
    kSortOrderValue,
    kSortOrderPosition
  } SortOrder;

  virtual size_t OnInputFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in, Formats::FixedArray<2> *out)
      const noexcept override;

 private:
  static constexpr int kDefaultPeaksNumber = 3;

  int peaks_number_;
  SortOrder order_;
  ExtremumType type_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_PEAK_DETECTION
