/*! @file autocorrelation.cc
 *  @brief Find the cross-correlation of a signal with itself.
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

#include "src/transforms/autocorrelation.h"
#include <simd/arithmetic-inl.h>
#include <simd/correlate.h>
#include <fftf/api.h>
#include <mutex>

namespace sound_feature_extraction {
namespace transforms {

Autocorrelation::Autocorrelation() : normalize_(kDefaultNormalize) {
}

ALWAYS_VALID_TP(Autocorrelation, normalize)

void Autocorrelation::Initialize() const {
  // Workaround for SIGSEGV in libav FFT with sizes greater than 2^16
  if (input_format_->Size() > 32768) {
    fftf_set_backend_priority(FFTF_BACKEND_LIBAV, -1000);
    fftf_set_backend(FFTF_BACKEND_NONE);
  }
  correlation_handles_.resize(threads_number());
  for (int i = 0; i < threads_number(); i++) {
    correlation_handles_[i].handle = std::shared_ptr<CrossCorrelationHandle>(
        new CrossCorrelationHandle(cross_correlate_initialize(
            input_format_->Size(), input_format_->Size())),
        [](CrossCorrelationHandle *ptr) {
          cross_correlate_finalize(*ptr);
          delete ptr;
        });
  }
}

size_t Autocorrelation::OnFormatChanged(size_t buffersCount) {
  output_format_->SetSize(input_format_->Size() * 2 - 1);
  return buffersCount;
}

void Autocorrelation::Do(const float* in, float* out) const noexcept {
  bool executed = false;
  while (!executed) {
    for (auto& hp : correlation_handles_) {
      if (hp.mutex.try_lock()) {
        cross_correlate(*hp.handle, in, in, out);
        hp.mutex.unlock();
        if (normalize_) {
          float norm = 1 / out[input_format_->Size() - 1];
          real_multiply_scalar(out, output_format_->Size(), norm, out);
        }
        executed = true;
        break;
      }
    }
  }
}

RTP(Autocorrelation, normalize)
REGISTER_TRANSFORM(Autocorrelation);

}  // namespace transforms
}  // namespace sound_feature_extraction
