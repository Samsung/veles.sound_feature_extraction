/*! @file spectral_energy.cc
 *  @brief Calculate the magnitude of each complex number.
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

#include "src/transforms/spectral_energy.h"
#include <cmath>
#include <simd/instruction_set.h>

namespace sound_feature_extraction {
namespace transforms {

size_t SpectralEnergy::OnFormatChanged(size_t buffersCount) {
  if (input_format_->Size() % 2 == 1) {
    WRN("Input buffer size is odd (%zu), truncated\n",
        input_format_->Size());
  }
  output_format_->SetSize(input_format_->Size() / 2);
  return buffersCount;
}

void SpectralEnergy::Do(const float* in,
                        float* out) const noexcept {
  Do(use_simd(), in, input_format_->Size(), out);
}

void SpectralEnergy::Do(bool simd, const float* input, int length,
                float* output) noexcept {
  if (simd) {
#ifdef __AVX__
    for (int j = 0; j < length - 15; j += 16) {
      __m256 vec1 = _mm256_load_ps(input + j);
      __m256 vec2 = _mm256_load_ps(input + j + 8);
      vec1 = _mm256_mul_ps(vec1, vec1);
      vec2 = _mm256_mul_ps(vec2, vec2);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
      __m256 r1 = _mm256_permute2f128_ps(vec1, vec2, 0x20);
      __m256 r2 = _mm256_permute2f128_ps(vec1, vec2, 0x31);
#pragma GCC diagnostic pop
      __m256 res = _mm256_hadd_ps(r1, r2);
      _mm256_store_ps(output + j / 2, res);
    }
    for (int j = (length & ~0xF); j < length; j += 2) {
      float re = input[j];
      float im = input[j + 1];
      output[j / 2] = re * re + im * im;
    }
  } else {
#elif defined(__ARM_NEON__)
    for (int j = 0; j < length - 3; j += 4) {
      float32x4_t cvec = vld1q_f32(input + j);
      float32x4_t sqrvec = vmulq_f32(cvec, cvec);
      float32x2_t sums = vpadd_f32(vget_low_f32(sqrvec),
                                   vget_high_f32(sqrvec));
      vst1_f32(output + j / 2, sums);
    }
    for (int j = ((length >> 2) << 2); j < length; j += 2) {
      float re = input[j];
      float im = input[j + 1];
      output[j / 2] = re * re + im * im;
    }
  } else {
#else
  } {
#endif
    for (int j = 0; j < length; j += 2) {
      float re = input[j];
      float im = input[j + 1];
      output[j / 2] = re * re + im * im;
    }
  }
}

REGISTER_TRANSFORM(SpectralEnergy);

}  // namespace transforms
}  // namespace sound_feature_extraction
