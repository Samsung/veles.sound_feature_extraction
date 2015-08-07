/*! @file lpc.cc
 *  @brief Find Linear Prediction Coefficients (LPC) using Levinson-Durbin
 *  recursion.
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

#include "src/transforms/lpc.h"
#include "src/primitives/lpc.h"

namespace sound_feature_extraction {
namespace transforms {

LPC::LPC() : error_(kDefaultError) {
}

ALWAYS_VALID_TP(LPC, error)

size_t LPC::OnFormatChanged(size_t buffersCount) {
  if (!error_) {
    output_format_->SetSize(input_format_->Size() - 1);
  } else {
    output_format_->SetSize(input_format_->Size());
  }
  return buffersCount;
}

void LPC::Do(const float* in, float* out) const noexcept {
  float err = ldr_lpc(use_simd(), in, input_format_->Size(),
                      out + (error_? 1 : 0));
  if (error_) {
    out[0] = err;
  }
}

RTP(LPC, error)
REGISTER_TRANSFORM(LPC);

}  // namespace transforms
}  // namespace sound_feature_extraction
