/*! @file dct.cc
 *  @brief Discrete Cosine Transform calculation.
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

#include "src/transforms/dct.h"
#include <fftf/api.h>
#include <vector>
#include <simd/arithmetic-inl.h>

namespace sound_feature_extraction {
namespace transforms {

void DCT::Do(const BuffersBase<float*>& in,
             BuffersBase<float*>* out) const noexcept {
  int length = output_format_->Size();
  std::vector<const float*> inputs(in.Count());
  std::vector<float*> outputs(in.Count());
  for (size_t i = 0; i < in.Count(); i++) {
    inputs[i] = in[i];
    outputs[i] = (*out)[i];
  }
  fftf_set_backend(FFTF_BACKEND_NONE);
  fftf_ensure_is_supported(FFTF_TYPE_DCT, input_format_->Size());
  auto fft_plan = std::unique_ptr<FFTFInstance, void (*)(FFTFInstance *)>(
      fftf_init_batch(
          FFTF_TYPE_DCT,
          FFTF_DIRECTION_FORWARD,
          FFTF_DIMENSION_1D,
          &length,
          FFTF_NO_OPTIONS,
          in.Count(),
          &inputs[0], &outputs[0]),
      fftf_destroy);

  fftf_calc(fft_plan.get());
}

void DCTInverse::Do(const BuffersBase<float*>& in,
                    BuffersBase<float*>* out) const noexcept {
  int length = output_format_->Size();
  std::vector<const float*> inputs(in.Count());
  std::vector<float*> outputs(in.Count());
  for (size_t i = 0; i < in.Count(); i++) {
    inputs[i] = in[i];
    outputs[i] = (*out)[i];
  }
  fftf_set_backend(FFTF_BACKEND_NONE);
  fftf_ensure_is_supported(FFTF_TYPE_DCT, input_format_->Size());
  auto fft_plan = std::unique_ptr<FFTFInstance, void (*)(FFTFInstance *)>(
      fftf_init_batch(
          FFTF_TYPE_DCT,
          FFTF_DIRECTION_BACKWARD,
          FFTF_DIMENSION_1D,
          &length,
          FFTF_NO_OPTIONS,
          in.Count(),
          &inputs[0], &outputs[0]),
      fftf_destroy);

  fftf_calc(fft_plan.get());
  for (size_t i = 0; i < in.Count(); i++) {
    real_multiply_scalar(outputs[i], length, 0.5f / length, outputs[i]);
  }
}

REGISTER_TRANSFORM(DCT);
REGISTER_TRANSFORM(DCTInverse);

}  // namespace transforms
}  // namespace sound_feature_extraction
