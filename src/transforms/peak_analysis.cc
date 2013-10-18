/*! @file peak_analysis.cc
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

#include "src/transforms/peak_analysis.h"
#include <algorithm>

namespace SoundFeatureExtraction {
namespace Transforms {

size_t PeakAnalysis::OnInputFormatChanged(size_t buffersCount) {
  outputFormat_->SetSize(inputFormat_->Size() * 2);
  return buffersCount;
}

void PeakAnalysis::Do(const Formats::FixedArray<2>* in, float *out)
    const noexcept {
  out[0] = in[0][0];
  float sum = in[0][1];
  for (size_t i = 1; i < inputFormat_->Size(); i++) {
    sum += in[i][1];
  }
  out[1] = in[0][1] / sum;
  for (size_t i = 1; i < inputFormat_->Size(); i++) {
    out[i * 2] = in[i][0] / in[0][0];
    out[i * 2 + 1] = in[i][1] / sum;
  }
}

REGISTER_TRANSFORM(PeakAnalysis);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
