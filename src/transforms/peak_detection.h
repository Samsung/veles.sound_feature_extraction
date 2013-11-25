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

#ifndef SRC_TRANSFORMS_PEAK_DETECTION_H_
#define SRC_TRANSFORMS_PEAK_DETECTION_H_

#include <mutex>
#include <vector>
#include <simd/detect_peaks.h>
#include <simd/wavelet_types.h>
#include "src/formats/fixed_array.h"
#include "src/primitives/wavelet_filter_bank.h"
#include "src/transforms/common.h"

namespace sound_feature_extraction {

enum SortOrder {
  kSortOrderValue = 1,
  kSortOrderPosition = 2,
  kSortOrderBoth = 3
};

namespace internal {
constexpr const char* kSortOrderValueStr = "value";
constexpr const char* kSortOrderPositionStr = "position";
constexpr const char* kSortOrderBothStr = "both";
constexpr const char* kExtremumTypeMinimumStr = "min";
constexpr const char* kExtremumTypeMaximumStr = "max";
constexpr const char* kExtremumTypeBothStr = "all";
}

SortOrder Parse(const std::string& value, identity<SortOrder>);
ExtremumType Parse(const std::string& value, identity<ExtremumType>);

namespace transforms {

class PeakDetection : public OmpTransformBase<
    formats::ArrayFormatF, formats::ArrayFormat<formats::FixedArray<2>>> {
 public:
  PeakDetection();

  TRANSFORM_INTRO("Peaks", "Peak detection, that is, extraction of maximums "
                  "and/or minimums from the signal. The first item is the "
                  "normalized position, the second is the peak value.",
                  PeakDetection)

  TP(sort, SortOrder, kDefaultSortOrder,
     "The order in which to sort peaks (position, value, or both).")
  TP(number, int, kDefaultPeaksNumber,
     "The maximal number of peaks to extract. The rest of the slots will "
     "be set to (min_pos, 0).")
  TP(type, ExtremumType, kDefaultExtremumType,
     "The type of the peaks: max, min or all.")
  TP(min_pos, float, kDefaultMinPos,
     "The real minimal value of the position of each peak.")
  TP(max_pos, float, kDefaultMaxPos,
     "The real maximal value of the position of each peak.")
  TP(swt_type, WaveletType, kDefaultSWTType,
     "Type of Stationary Wavelet Transform which will be done "
     " before taking the peaks. Supported values are "
     "daub (Daubechies), coif (Coiflet) and sym (Symlet). "
     "Please note that SWT introduces a phase shift.")
  TP(swt_order, int, kDefaultWaveletOrder,
     "The number of coefficients in the wavelet.")
  TP(swt_level, int, kDefaultSWTLevel,
     "SWT level, that is, the number of smoothing passes."
     "\"0\" means do not do SWT.")

 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override;

  virtual void Initialize() const override;

  virtual void Do(const float* in, formats::FixedArray<2>* out)
      const noexcept override;

 protected:
  static constexpr SortOrder kDefaultSortOrder = kSortOrderBoth;
  static constexpr int kDefaultPeaksNumber = 3;
  static constexpr ExtremumType kDefaultExtremumType = kExtremumTypeMaximum;
  static constexpr float kDefaultMinPos = 0;
  static constexpr float kDefaultMaxPos = 1;
  static constexpr WaveletType kDefaultSWTType = WAVELET_TYPE_DAUBECHIES;
  static constexpr int kDefaultWaveletOrder = 4;
  static constexpr int kDefaultSWTLevel = 0;

 private:
  mutable FloatPtr swt_details_buffer_;
  mutable std::vector<ThreadSafeFloatPtr> swt_buffers_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction

namespace std {
  inline string
  to_string(sound_feature_extraction::SortOrder value) noexcept {
    switch (value) {
      case sound_feature_extraction::kSortOrderValue:
        return sound_feature_extraction::internal::kSortOrderValueStr;
      case sound_feature_extraction::kSortOrderPosition:
        return sound_feature_extraction::internal::kSortOrderPositionStr;
      case sound_feature_extraction::kSortOrderBoth:
        return sound_feature_extraction::internal::kSortOrderBothStr;
    }
    return "";
  }

  inline string
  to_string(ExtremumType value) noexcept {
    switch (value) {
      case kExtremumTypeMinimum:
        return sound_feature_extraction::internal::kExtremumTypeMinimumStr;
      case kExtremumTypeMaximum:
        return sound_feature_extraction::internal::kExtremumTypeMaximumStr;
      case kExtremumTypeBoth:
        return sound_feature_extraction::internal::kExtremumTypeBothStr;
    }
    return "";
  }
}  // namespace std

#endif  // SRC_TRANSFORMS_PEAK_DETECTION_H_
