/*! @file log.cc
 *  @brief Taking logarithm on the values.
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

#include "src/transforms/log.h"
#include <cmath>
#ifdef __AVX__
#include <simd/avx_mathfun.h>
#elif defined(__ARM_NEON__)
#include <simd/neon_mathfun.h>
#endif

namespace sound_feature_extraction {
namespace transforms {

LogarithmBase Parse(const std::string& value, identity<LogarithmBase>) {
  static const std::unordered_map<std::string, LogarithmBase> map {
    { internal::kLogBaseEStr, LogarithmBase::kE },
    { internal::kLogBase2Str, LogarithmBase::k2 },
    { internal::kLogBase10Str, LogarithmBase::k10 }
  };
  auto lbit = map.find(value);
  if (lbit == map.end()) {
    throw InvalidParameterValueException();
  }
  return lbit->second;
}

void LogRaw::Do(bool simd, const float* input, int length,
                float* output) const noexcept {
  bool vadd1 = add1();
  float vscale = scale();
  switch (base()) {
    case LogarithmBase::kE: {
      if (simd) {
#ifdef __AVX__
        for (int j = 0; j < length - 7; j += 8) {
          __m256 vec = _mm256_load_ps(input + j);
          if (vscale != 1.f) {
            vec = _mm256_mul_ps(vec, _mm256_set1_ps(vscale));
          }
          if (vadd1) {
            vec = _mm256_add_ps(vec, _mm256_set1_ps(1.f));
          }
          vec = log256_ps(vec);
          _mm256_store_ps(output + j, vec);
        }
        for (int j = (length & ~0x7); j < length; j++) {
          output[j] = logf(input[j] * vscale + vadd1);
        }
      } else {
#elif defined(__ARM_NEON__)
        int length = input_format_->Size();
        for (int j = 0; j < length - 3; j += 4) {
          float32x4_t vec = vld1q_f32(input + j);
          if (vscale != 1.f) {
            vec = vmulq_f32(vec, vdupq_n_f32(vscale));
          }
          if (vadd1) {
            vec = vaddq_f32(vec, vdupq_n_f32(1.f));
          }
          vec = log_ps(vec);
          vst1q_f32(output + j, vec);
        }
        for (int j = (length & ~0x3); j < length; j++) {
          output[j] = logf(input[j] * vscale + vadd1);
        }
      } else {
#else
      } {
#endif
        for (int j = 0; j < length; j++) {
          output[j] = logf(input[j] * vscale + vadd1);
        }
      }
      break;
    }
    case LogarithmBase::k2:
      for (int j = 0; j < length; j++) {
        output[j] = log2f(input[j] * vscale + vadd1);
      }
      break;
    case LogarithmBase::k10:
      for (int j = 0; j < length; j++) {
        output[j] = log10f(input[j] * vscale + vadd1);
      }
      break;
  }
}

void LogRaw::Do(const float* in, float* out) const noexcept {
  Do(use_simd(), in, this->input_format_->Size(), out);
}

void LogRawInverse::Do(const float* in UNUSED, float* out UNUSED)
    const noexcept {
  assert("Not implemented yet");
}

void LogSingle::Do(const float& in, float* out) const noexcept {
  float val = in;
  val *= scale();
  val += add1();
  switch (base()) {
    case LogarithmBase::kE:
      *out = logf(val);
      break;
    case LogarithmBase::k2:
      *out = log2f(val);
      break;
    case LogarithmBase::k10:
      *out = log10f(val);
      break;
  }
}

void LogSingleInverse::Do(const float& in UNUSED, float* out UNUSED)
    const noexcept {
  assert("Not implemented yet");
}

REGISTER_TRANSFORM(LogRaw);
REGISTER_TRANSFORM(LogSingle);
REGISTER_TRANSFORM(LogRawInverse);
REGISTER_TRANSFORM(LogSingleInverse);

}  // namespace transforms
}  // namespace sound_feature_extraction
