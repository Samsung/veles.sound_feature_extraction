/*! @file unpack_rdft.cc
 *  @brief Unpacks data after applying RDFT.
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

#include "src/transforms/unpack_rdft.h"

namespace sound_feature_extraction {
namespace transforms {

size_t UnpackRDFT::OnFormatChanged(size_t buffersCount) {
  if (input_format_->Size() % 2 == 1) {
    output_format_->SetSize((input_format_->Size() - 1) * 2);
  } else {
    output_format_->SetSize((input_format_->Size() - 2) * 2);
  }
  return buffersCount;
}

void UnpackRDFT::Initialize() const {
  realMode_ = input_format_->Size() % 2 == 1;
  offset_ = input_format_->Size();
  length_ = output_format_->Size() - offset_;
}

void UnpackRDFT::Do(const float* in,
                    float* out) const noexcept {
  if (in != out) {
    memcpy(out, in, offset_ * sizeof(in[0]));
  }
  if (realMode_) {
    rmemcpyf(out + offset_, in + 1, length_);
  } else {
    crmemcpyf(out + offset_, in + 2, length_);
  }
}

REGISTER_TRANSFORM(UnpackRDFT);

}  // namespace transforms
}  // namespace sound_feature_extraction
