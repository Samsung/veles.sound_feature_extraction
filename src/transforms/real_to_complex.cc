/*! @file real_to_complex.cc
 *  @brief Converts real numbers to the corresponding complex numbers.
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

#include "src/transforms/real_to_complex.h"
#include <simd/instruction_set.h>

namespace sound_feature_extraction {
namespace transforms {

size_t RealToComplex::OnFormatChanged(size_t buffersCount) {
  output_format_->SetSize(input_format_->Size() * 2);
  return buffersCount;
}

void RealToComplex::Do(const float* in,
                       float* out) const noexcept {
  Do(use_simd(), in, input_format_->Size(), out);
}

void RealToComplex::Do(bool simd, const float* input, int length,
                       float* output) noexcept {
  if (simd) {
#ifdef __AVX__
#ifndef __AVX2__
    for (int i = 0; i < length - 7; i += 8) {
      __m256 vec = _mm256_load_ps(input + i);
      __m256 low = _mm256_unpacklo_ps(vec, _mm256_setzero_ps());
      __m256 high = _mm256_unpackhi_ps(vec, _mm256_setzero_ps());
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
      __m256 reslow = _mm256_permute2f128_ps(low, high, 32);
      __m256 reshigh = _mm256_permute2f128_ps(low, high, 49);
#pragma GCC diagnostic pop
      _mm256_store_ps(output + 2 * i, reslow);
      _mm256_store_ps(output + 2 * i + 8, reshigh);
    }
    for (int i = ((length >> 3) << 3); i < length; i++) {
      output[i * 2] = input[i];
      output[i * 2 + 1] = 0.f;
    }
#else
#error TODO: AVX2 introduces a full 256-bit permute which must be executed before _mm256_unpack*  // NOLINT(*)
#endif
  } else {
#elif defined(__ARM_NEON__)
    const float32x4_t zeros = { 0.f, 0.f, 0.f, 0.f };
    for (int i = 0; i < length - 3; i += 4) {
      float32x4_t vec = vld1q_f32(input + i);
      float32x4x2_t res = vzipq_f32(vec, zeros);
      vst1q_f32(output + i * 2, res.val[0]);
      vst1q_f32(output + i * 2 + 4, res.val[1]);
    }

    for (int i = ((length >> 2) << 2); i < length; i++) {
      output[i * 2] = input[i];
      output[i * 2 + 1] = 0.f;
    }
  } else {
#else
  } {
#endif
    for (int i = 0; i < length; i++) {
      output[i * 2] = input[i];
      output[i * 2 + 1] = 0.f;
    }
  }
}

REGISTER_TRANSFORM(RealToComplex);

}  // namespace transforms
}  // namespace sound_feature_extraction
