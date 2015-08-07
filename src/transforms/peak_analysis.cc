/*! @file peak_analysis.cc
 *  @brief Peaks analysis.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 *
 *  @section License
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */

#include "src/transforms/peak_analysis.h"
#include <algorithm>

namespace sound_feature_extraction {
namespace transforms {

size_t PeakAnalysis::OnInputFormatChanged(size_t buffersCount) {
  output_format_->SetSize(input_format_->Size() * 2 + 1);
  return buffersCount;
}

void PeakAnalysis::Do(const formats::FixedArray<2>* in, float *out)
    const noexcept {
  float sum = in[0][1];
  for (size_t i = 1; i < input_format_->Size(); i++) {
    sum += in[i][1];
  }
  if (sum == 0) {
    sum = 1;
  }
  float max_pos = in[0][0];
  float max_val = in[0][1];
  for (size_t i = 1; i < input_format_->Size(); i++) {
    if (in[i][1] > max_val) {
      max_val = in[i][1];
      max_pos = in[i][0];
    }
  }
  if (max_pos == 0) {
    max_pos = 1;
  }
  for (size_t i = 0; i < input_format_->Size(); i++) {
    out[i * 2 + 1] = in[i][0] / max_pos;
    out[i * 2 + 2] = in[i][1] / sum;
  }
  out[0] = max_pos;
}

REGISTER_TRANSFORM(PeakAnalysis);

}  // namespace transforms
}  // namespace sound_feature_extraction
