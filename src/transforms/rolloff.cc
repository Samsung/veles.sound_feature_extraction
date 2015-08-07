/*! @file rolloff.cc
 *  @brief Measure of spectral shape.
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

#include "src/transforms/rolloff.h"
#include <simd/arithmetic-inl.h>

namespace sound_feature_extraction {
namespace transforms {

Rolloff::Rolloff() : ratio_(kDefaultRatio) {
}

bool Rolloff::validate_ratio(const float& value) noexcept {
  return value > 0.f && value < 1.f;
}

void Rolloff::Do(const float* in, float* out) const noexcept {
  *out = Do(use_simd(), in, input_format_->Size(), ratio_) /
      input_format_->Duration();
}

int Rolloff::Do(bool simd, const float* input, size_t length,
                float ratio) noexcept {
  int ilength = length;
  float threshold;
  if (simd) {
#if defined(__AVX__) || defined(__ARM_NEON__)
    threshold = sum_elements(input, length) * ratio;
    int j;
    float psum = 0.f;
#ifdef __AVX__
    int max_index = ilength - 15;
    for (j = 0; j < max_index && psum < threshold; j += 16) {
      __m256 vec1 = _mm256_load_ps(input + j);
      __m256 vec2 = _mm256_load_ps(input + j + 8);
      __m256 psumvec = _mm256_hadd_ps(vec1, vec2);
      psumvec = _mm256_hadd_ps(psumvec, psumvec);
      psumvec = _mm256_hadd_ps(psumvec, psumvec);
      psum += _mm256_get_ps(psumvec, 0) + _mm256_get_ps(psumvec, 4);
    }
    if (j < max_index) {
      int i;
      for (i = j - 1; i > j - 16 && psum > threshold; i--) {
        psum -= input[i];
      }
      return i + 1;
    }
    int i;
    for (i = j - 16; i < ilength && psum < threshold; i++) {
      psum += input[i];
    }
    return i - 1;
#else
    int max_index = ilength - 7;
    for (j = 0; j < max_index && psum < threshold; j += 8) {
      float32x4_t vec1 = vld1q_f32(input + j);
      float32x4_t vec2 = vld1q_f32(input + j + 4);
      float32x4_t psumvec = vaddq_f32(vec1, vec2);
      float32x2_t psumvec2 = vpadd_f32(vget_high_f32(psumvec),
                                       vget_low_f32(psumvec));
      psum += vget_lane_f32(psumvec2, 0) + vget_lane_f32(psumvec2, 1);
    }
    if (j < max_index) {
      int i;
      for (i = j - 1; i > j - 8 && psum > threshold; i--) {
        psum -= input[i];
      }
      return i + 1;
    }
    int i;
    for (i = j - 8; i < ilength && psum < threshold; i++) {
      psum += input[i];
    }
    return i - 1;
#endif
  } else {
#else
  } {
#endif
    threshold = sum_elements_na(input, length) * ratio;
    float psum = 0.f;
    int i;
    for (i = 0; i < ilength && psum < threshold; i++) {
      psum += input[i];
    }
    return i - 1;
  }
}

RTP(Rolloff, ratio)
REGISTER_TRANSFORM(Rolloff);

}  // namespace transforms
}  // namespace sound_feature_extraction
