/*! @file delta.cc
 *  @brief New file description.
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

#include "src/transforms/delta.h"
#include <simd/arithmetic-inl.h>

namespace sound_feature_extraction {
namespace transforms {

DeltaType Parse(const std::string& value, identity<DeltaType>) {
  static const std::unordered_map<std::string, DeltaType> map {
    { internal::kDeltaTypeSimpleStr, DeltaType::kSimple },
    { internal::kDeltaTypeRegressionStr, DeltaType::kRegression }
  };
  auto dti = map.find(value);
  if (dti == map.end()) {
    throw InvalidParameterValueException();
  }
  return dti->second;
}

constexpr DeltaType Delta::kDefaultDeltaType;

Delta::Delta()
    : type_(kDefaultDeltaType),
      rlength_(kDefaultRegressionLength) {
}

ALWAYS_VALID_TP(Delta, type)
bool Delta::validate_rlength(const int& value) noexcept {
  return value >= 3 && (value % 2) == 1;
}

void Delta::Do(const BuffersBase<float*>& in,
               BuffersBase<float*>* out) const noexcept {
  switch (type_) {
    case DeltaType::kSimple:
      for (size_t i = 1; i < in.Count(); i++) {
        DoSimple(use_simd(), in[i - 1], in[i],
                 input_format_->Size(), (*out)[i]);
      }
      for (size_t i = 0; i < input_format_->Size(); i++) {
        (*out)[0][i] = (*out)[1][i];
      }
    break;
    case DeltaType::kRegression: {
      int rstep = rlength_ / 2;
      float norm = 2 * rstep * (rstep + 1) * (2 * rstep + 1) / 6;
      for (size_t i = rstep; i < in.Count() - rstep; i++) {
        DoRegression(use_simd(), in, rstep, i, norm, input_format_->Size(),
                     out);
      }
      for (size_t i = rstep - 1; i > 0; i--) {
        norm = 2 * i * (i + 1) * (2 * i + 1) / 6;
        DoRegression(use_simd(), in, i, i, norm, input_format_->Size(), out);
        DoRegression(use_simd(), in, in.Count() - i, i, norm,
                     input_format_->Size(), out);
      }
      for (size_t i = 0; i < input_format_->Size(); i++) {
        (*out)[0][i] = (*out)[1][i];
        (*out)[in.Count() - 1][i] = (*out)[in.Count() - 2][i];
      }
      break;
    }
  }
}

void Delta::DoSimple(bool simd, const float* prev, const float* cur,
                     size_t length, float* res) noexcept {
  int ilength = length;
  if (simd) {
#ifdef __AVX__
    for (int i = 0; i < ilength - 15; i += 16) {
      __m256 vecp1 = _mm256_load_ps(prev + i);
      __m256 vecp2 = _mm256_load_ps(prev + i + 8);
      __m256 vecc1 = _mm256_load_ps(cur + i);
      __m256 vecc2 = _mm256_load_ps(cur + i + 8);
      __m256 diff1 = _mm256_sub_ps(vecc1, vecp1);
      __m256 diff2 = _mm256_sub_ps(vecc2, vecp2);
      _mm256_store_ps(res + i, diff1);
      _mm256_store_ps(res + i + 8, diff2);
    }
    for (int i = (ilength & ~0xF); i < ilength; i++) {
      res[i] = cur[i] - prev[i];
    }
    return;
  } else {
#elif defined(__ARM_NEON__)
    for (int i = 0; i < ilength - 7; i += 8) {
      float32x4_t vecp1 = vld1q_f32(prev + i);
      float32x4_t vecp2 = vld1q_f32(prev + i + 4);
      float32x4_t vecc1 = vld1q_f32(cur + i);
      float32x4_t vecc2 = vld1q_f32(cur + i + 4);
      float32x4_t diff1 = vsubq_f32(vecc1, vecp1);
      float32x4_t diff2 = vsubq_f32(vecc2, vecp2);
      vst1q_f32(res + i, diff1);
      vst1q_f32(res + i + 4, diff2);
    }
    for (int i = (ilength & ~0x7); i < ilength; i++) {
      res[i] = cur[i] - prev[i];
    }
    return;
  } else {
#else
  } {
#endif
    for (int i = 0; i < ilength; i++) {
      res[i] = cur[i] - prev[i];
    }
    return;
  }
}

void Delta::DoRegression(bool simd, const BuffersBase<float*>& in,
                         int rstep, int i, float norm, int windowSize,
                         BuffersBase<float*>* out) noexcept {
  assert(false && "Unstable implementation of Delta regression");
  if (simd) {
#ifdef __AVX__
    __m256 normvec = _mm256_set1_ps(norm);
    for (int j = 0; j < windowSize - 7; j += 8) {
      __m256 sum = _mm256_setzero_ps();
      __m256 kvec = _mm256_set1_ps(1.f);
      for (int k = 1; k <= rstep; k++) {
        __m256 rvec = _mm256_load_ps(in[i + k] + j);
        __m256 lvec = _mm256_load_ps(in[i - k] + j);
        __m256 diff = _mm256_sub_ps(rvec, lvec);
        diff = _mm256_mul_ps(diff, kvec);
        sum = _mm256_add_ps(sum, diff);
        kvec = _mm256_add_ps(kvec, _mm256_set1_ps(1.f));
      }
      sum = _mm256_div_ps(sum, normvec);
      _mm256_store_ps((*out)[i] + j, sum);
    }
    for (int j = windowSize & ~7; j < windowSize; j++) {
      float sum = 0.f;
      for (int k = 1; k <= rstep; k++) {
        sum += (in[i + k][j] - in[i - k][j]) * k;
      }
      (*out)[i][j] = sum / norm;
    }
    return;
  } else {
#elif defined(__ARM_NEON__)
    float32x4_t normvec = vdupq_n_f32(1.f / norm);
    for (int j = 0; j < windowSize - 3; j += 4) {
      float32x4_t sum = vdupq_n_f32(0.f);
      float32x4_t kvec = vdupq_n_f32(1.f);
      for (int k = 1; k <= rstep; k++) {
        float32x4_t rvec = vld1q_f32(in[i + k] + j);
        float32x4_t lvec = vld1q_f32(in[i - k] + j);
        float32x4_t diff = vsubq_f32(rvec, lvec);
        sum = vmlaq_f32(diff, kvec, sum);
        kvec = vaddq_f32(kvec, vdupq_n_f32(1.f));
      }
      sum = vmulq_f32(sum, normvec);
      vst1q_f32((*out)[i] + j, sum);
    }
    for (int j = windowSize & ~3; j < windowSize; j++) {
      float sum = 0.f;
      for (int k = 1; k <= rstep; k++) {
        sum += (in[i + k][j] - in[i - k][j]) * k;
      }
      (*out)[i][j] = sum / norm;
    }
    return;
  } else {
#else
  } {
#endif
    for (int j = 0; j < windowSize; j++) {
      float sum = 0.f;
      for (int k = 1; k <= rstep; k++) {
        sum += (in[i + k][j] - in[i - k][j]) * k;
      }
      (*out)[i][j] = sum / norm;
    }
  }
}

RTP(Delta, rlength)
RTP(Delta, type)
REGISTER_TRANSFORM(Delta);

}  // namespace transforms
}  // namespace sound_feature_extraction
