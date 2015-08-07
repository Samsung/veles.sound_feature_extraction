/*! @file subsampling.cc
 *  @brief Subsample the signal by an integer factor.
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

#include "src/transforms/subsampling.h"
#include "src/transforms/complex_to_real.h"

namespace sound_feature_extraction {
namespace transforms {

Subsampling::Subsampling() : factor_(kDefaultFactor) {
}

bool Subsampling::validate_factor(const int& value) noexcept {
  return value >= 1;
}

size_t Subsampling::OnFormatChanged(size_t buffersCount) {
  output_format_->SetSize(input_format_->Size() / factor_);
  output_format_->SetSamplingRate(input_format_->SamplingRate() / 2);
  return buffersCount;
}

void Subsampling::Do(const float* in, float* out) const noexcept {
  switch (factor_) {
    case 1:
      memcpy(out, in, input_format_->SizeInBytes());
      break;
    case 2:
      ComplexToReal::Do(use_simd(), in, input_format_->Size(), out);
      break;
    default:
      for (int i = 0; i < static_cast<int>(output_format_->Size()); i++) {
        out[i] = in[i * factor_];
      }
      break;
  }
}

RTP(Subsampling, factor)
REGISTER_TRANSFORM(Subsampling);

}  // namespace transforms
}  // namespace sound_feature_extraction
