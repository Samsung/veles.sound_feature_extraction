/*! @file zero_padding.cc
 *  @brief Pad signal with zeros to make it's length a power of 2.
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

#include "src/transforms/zero_padding.h"
#include <simd/arithmetic-inl.h>

namespace sound_feature_extraction {
namespace transforms {

size_t ZeroPadding::OnFormatChanged(size_t buffersCount) {
  int length = input_format_->Size();
  if ((length & (length - 1)) == 0) {
    output_format_->SetSize(length);
  } else {
    output_format_->SetSize(next_highest_power_of_2(length));
  }
  return buffersCount;
}

void ZeroPadding::Do(const float* in,
                     float* out) const noexcept {
  if (in != out) {
    memcpy(out, in, input_format_->Size() * sizeof(in[0]));
  }
  memsetf(out + input_format_->Size(),
          0.f,
          output_format_->Size() - input_format_->Size());
}

REGISTER_TRANSFORM(ZeroPadding);

}  // namespace transforms
}  // namespace sound_feature_extraction
