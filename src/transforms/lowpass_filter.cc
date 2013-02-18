/*! @file lowpass_filter.cc
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

#include "src/transforms/lowpass_filter.h"
#include <math.h>
#include "src/formats/format_limits.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

LowpassFilter::LowpassFilter() noexcept
: HammingWindowFilter(SupportedParameters())
, frequency_(DEFAULT_FILTER_HIGH_FREQUENCY) {
}

void LowpassFilter::CalculateFilter(float *filter) const noexcept {
  const float wOffset = (length_ - 1) / 2.0f;
  int samplingRate = inputFormat_.SamplingRate();
  for (int n = 0; n < length_; n++) {
    float h;
    if (n - wOffset != 0) {
      h = sinf(2 * M_PI * (n - wOffset) * frequency_ / samplingRate) /
          ( M_PI * (n - wOffset));
    } else {
      // lim  sin (x) / x = 1, handle this case separately
      // x->0
      h = 2.0f * frequency_ / samplingRate;
    }
    filter[n] *= h;
  }
}

void LowpassFilter::SetFilterParameter(const std::string& name,
                                 const std::string& value) {
  if (name == "frequency") {
    int pv = Parse<int>(name, value);
    if (pv < MIN_FILTER_FREQUENCY || pv > MAX_FILTER_FREQUENCY) {
      throw new InvalidParameterValueException(name, value, Name());
    }
    frequency_ = pv;
  }
}

REGISTER_TRANSFORM(LowpassFilter);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
