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
  float sum = in[0][1];
  for (size_t i = 1; i < inputFormat_->Size(); i++) {
    sum += in[i][1];
  }
  if (sum == 0) {
    sum = 1;
  }
  float max_pos = in[0][0];
  float max_val = in[0][1];
  for (size_t i = 1; i < inputFormat_->Size(); i++) {
    if (in[i][1] > max_val) {
      max_val = in[i][1];
      max_pos = in[i][0];
    }
  }
  if (max_pos == 0) {
    max_pos = 1;
  }
  for (size_t i = 0; i < inputFormat_->Size(); i++) {
    out[i * 2] = in[i][0] / max_pos;
    out[i * 2 + 1] = in[i][1] / sum;
  }
  out[0] = in[0][0];
}

REGISTER_TRANSFORM(PeakAnalysis);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
