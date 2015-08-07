/*! @file fir_filter_base.cc
 *  @brief Base class for all FIR filters.
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

#include "src/transforms/fir_filter_base.h"
#include <simd/convolve.h>

namespace sound_feature_extraction {
namespace transforms {

void FIRFilterBase::Initialize() const {
  filter_.resize(length());
  CalculateFilter(filter_.data());

  FilterBase<ConvolutionHandle>::Initialize();
}

size_t FIRFilterBase::OnFormatChanged(size_t buffersCount) {
  output_format_->SetSize(input_format_->Size() + length() - 1);
  return buffersCount;
}

std::shared_ptr<ConvolutionHandle> FIRFilterBase::CreateExecutor()
    const noexcept {
  return std::shared_ptr<ConvolutionHandle>(
      new ConvolutionHandle(convolve_initialize(input_format_->Size(),
                                                filter_.size())),
      [](ConvolutionHandle* ptr) {
        convolve_finalize(*ptr);
        delete ptr;
      });
}

void FIRFilterBase::Execute(const std::shared_ptr<ConvolutionHandle>& exec,
                            const float* in, float* out) const {
  convolve(*exec, in, &filter_[0], out);
}

}  // namespace formats
}  // namespace sound_feature_extraction
