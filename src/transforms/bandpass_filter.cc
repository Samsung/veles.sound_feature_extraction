/*! @file highpass_filter.cc
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

#include "src/transforms/bandpass_filter.h"
#include <math.h>
#include "src/formats/format_limits.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

BandpassFilter::BandpassFilter() noexcept
: HammingWindowFilter(SupportedParameters())
, frequencyHigh_(DEFAULT_FILTER_HIGH_FREQUENCY)
, frequencyLow_(DEFAULT_FILTER_LOW_FREQUENCY){
}

void BandpassFilter::CalculateFilter(float *filter) const noexcept {
  const float wOffset = (length_ - 1) / 2.0f;
  int samplingRate = inputFormat_.SamplingRate();
  for (int n = 0; n < length_; n++) {
    float h;
    if (n - wOffset != 0) {
      h = sinf(2 * M_PI * (n - wOffset) * frequencyHigh_ / samplingRate) /
          ( M_PI * (n - wOffset)) -
          sinf(2 * M_PI * (n - wOffset) * frequencyLow_ / samplingRate) /
          ( M_PI * (n - wOffset));
    } else {
      h = 2.0f * (frequencyHigh_ - frequencyLow_) / samplingRate;
    }
    filter[n] *= h;
  }
}

void BandpassFilter::SetFilterParameter(const std::string& name,
                                        const std::string& value) {
  if (name == "frequency_high") {
    int pv = Parse<int>(name, value);
    if (pv < MIN_FILTER_FREQUENCY || pv > MAX_FILTER_FREQUENCY) {
      throw new InvalidParameterValueException(name, value, Name());
    }
    frequencyHigh_ = pv;
  } else if (name == "frequency_low") {
    int pv = Parse<int>(name, value);
    if (pv < MIN_FILTER_FREQUENCY || pv > MAX_FILTER_FREQUENCY) {
      throw new InvalidParameterValueException(name, value, Name());
    }
    frequencyLow_ = pv;
  }
}

REGISTER_TRANSFORM(BandpassFilter);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
