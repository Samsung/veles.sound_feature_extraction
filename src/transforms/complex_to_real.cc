/*! @file complex_to_real.cc
 *  @brief Converts complex numbers to the corresponding real numbers.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
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

#include "src/transforms/complex_to_real.h"
#include <simd/instruction_set.h>

namespace sound_feature_extraction {
namespace transforms {

size_t ComplexToReal::OnFormatChanged(size_t buffersCount) {
  output_format_->SetSize(input_format_->Size() / 2);
  return buffersCount;
}

void ComplexToReal::Do(const float* in, float* out) const noexcept {
  Do(use_simd(), in, input_format_->Size(), out);
}

void ComplexToReal::Do(bool simd, const float* input, int length,
                       float* output) noexcept {
  if (simd) {
#ifdef __AVX__
    for (int i = 0; i < length - 15; i += 16) {
      __m256 vec1 = _mm256_load_ps(input + i);
      __m256 vec2 = _mm256_load_ps(input + i + 8);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
      __m256 vec2even = _mm256_shuffle_ps(vec2, vec2, 160);
      __m256 low = _mm256_permute2f128_ps(vec1, vec2even, 32);
      __m256 high = _mm256_permute2f128_ps(vec1, vec2even, 49);
      __m256 result = _mm256_shuffle_ps(low, high, 136);
#pragma GCC diagnostic pop
      _mm256_store_ps(output + i / 2, result);
    }
    for (int i = (length & ~0xF); i < length; i += 2) {
      output[i / 2] = input[i];
    }
  } else {
#elif defined(__ARM_NEON__)
    for (int i = 0; i < length - 7; i += 8) {
      float32x4x2_t result = vld2q_f32(input + i);
      vst1q_f32(output + i / 2, result.val[0]);
    }
    for (int i = (length & ~0x7); i < length; i += 2) {
      output[i / 2] = input[i];
    }
  } else {
#else
  } {
#endif
    for (int i = 0; i < length; i += 2) {
      output[i / 2] = input[i];
    }
  }
}

REGISTER_TRANSFORM(ComplexToReal);

}  // namespace transforms
}  // namespace sound_feature_extraction
