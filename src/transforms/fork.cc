/*! @file fork.cc
 *  @brief Window cloning.
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

#include "src/transforms/fork.h"

namespace sound_feature_extraction {
namespace transforms {

Fork::Fork() : factor_(kDefaultFactor) {
}

bool Fork::validate_factor(const int& value) noexcept {
  return value >= 1;
}

size_t Fork::OnFormatChanged(size_t buffersCount) {
  return factor_ * buffersCount;
}

void Fork::Do(
    const BuffersBase<float*>& in,
    BuffersBase<float*>* out) const noexcept {
  size_t copy_size = input_format_->Size() * sizeof(in[0][0]);
  for (size_t i = 0, j = 0; i < in.Count(); i++, j += factor_) {
    auto input = in[i];
    for (int k = 0; k < factor_; k++) {
      memcpy((*out)[j + k], input, copy_size);
    }
  }
}

RTP(Fork, factor)
REGISTER_TRANSFORM(Fork);

}  // namespace transforms
}  // namespace sound_feature_extraction
