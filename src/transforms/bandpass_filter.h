/*! @file bandpass_filter.h
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_BANDPASS_FILTER_H_
#define SRC_TRANSFORMS_BANDPASS_FILTER_H_

#include "src/transforms/fir_filter_base.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

/// @brief Discards the frequencies which are lower than the threshold.
class BandpassFilter : public FirFilterBase {
 public:
  BandpassFilter() noexcept;

  TRANSFORM_INTRO("Bandpass", "Bandpass filter. Cuts off frequences higher "
                              "than \"frequency_high\" and lower than "
                              "\"frequency_low\".")

  FIR_FILTER_PARAMETERS(
      TP("frequency_high", "All frequencies higher will be cut",
         std::to_string(DEFAULT_FILTER_HIGH_FREQUENCY))
      TP("frequency_low", "All frequencies lower will be cut",
         std::to_string(DEFAULT_FILTER_LOW_FREQUENCY))
  )

 protected:
  virtual void CalculateFilter(float *filter) const noexcept;

 private:
  int frequencyHigh_;
  int frequencyLow_;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_BANDPASS_FILTER_H_
