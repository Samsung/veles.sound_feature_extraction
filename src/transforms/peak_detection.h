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

#include <mutex>
#include <vector>
#include <simd/detect_peaks.h>
#include <simd/wavelet_types.h>
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
      TP("sort", "The order in which to sort peaks (position, value, or both).",
         "both")
      TP("number", "The maximal number of peaks to extract. The rest of the "
                   "slots will be set to (min_pos, 0).",
         std::to_string(kDefaultPeaksNumber))
      TP("type", "The type of the peaks: max, min or all.", "max")
      TP("min_pos", "The real minimal value of the position of each peak.",
         "0")
      TP("max_pos", "The real maximal value of the position of each peak.",
         "1")
      TP("swt_type", "Type of Stationary Wavelet Transform which will be done "
                     " before taking the peaks. Supported values are "
                     "daub (Daubechies), coif (Coiflet) and sym (Symlet). "
                     "Please note that SWT introduces a phase shift.",
         "daub")
      TP("swt_order", "The number of coefficients in the wavelet.",
         std::to_string(kDefaultWaveletOrder))
      TP("swt_level", "SWT level, that is, the number of smoothing passes."
                      "\"0\" means do not do SWT.",
         "0")
  )

 protected:
  typedef enum {
    kSortOrderValue = 1,
    kSortOrderPosition = 2,
    kSortOrderBoth = 3
  } SortOrder;

  virtual size_t OnInputFormatChanged(size_t buffersCount) override;

  virtual void Initialize() const override;

  virtual void Do(const float* in, Formats::FixedArray<2>* out)
      const noexcept override;

 private:
  struct SWTBuffer {
    SWTBuffer() : data(nullptr, std::free) {
    }

    SWTBuffer(const SWTBuffer&) : data(nullptr, std::free) {
    }

    FloatPtr data;
    std::mutex mutex;
  };

  static constexpr int kDefaultPeaksNumber = 3;
  static constexpr int kDefaultWaveletOrder = 4;

  int peaks_number_;
  SortOrder order_;
  ExtremumType type_;
  float min_pos_;
  float max_pos_;
  mutable FloatPtr swt_details_buffer_;
  mutable std::vector<SWTBuffer> swt_buffers_;
  WaveletType swt_type_;
  int swt_order_;
  int swt_level_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_PEAK_DETECTION
