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

#ifndef BANDPASS_FILTER_H_
#define BANDPASS_FILTER_H_

#include "src/transforms/hamming_window_filter.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

/// @brief Discards the frequencies which are lower than the threshold.
class BandpassFilter : public HammingWindowFilter {
 public:
  BandpassFilter() noexcept;

  TRANSFORM_NAME("Bandpass")

  FIR_FILTER_PARAMETERS(
      _TP_("frequency_high", "All frequencies higher will be cut",
                             std::to_string(DEFAULT_FILTER_HIGH_FREQUENCY))
      _TP_("frequency_low", "All frequencies lower will be cut",
                            std::to_string(DEFAULT_FILTER_LOW_FREQUENCY))
  )

 protected:
  virtual void SetFilterParameter(const std::string& name,
                                  const std::string& value);

  virtual void CalculateFilter(float *filter) const noexcept;

 private:
  int frequencyHigh_;
  int frequencyLow_;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_BANDPASS_FILTER_H_
