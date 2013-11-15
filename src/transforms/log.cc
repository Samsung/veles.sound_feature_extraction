/*! @file log.cc
 *  @brief Taking logarithm on the values.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
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
    { internal::kLogBaseEStr, kLogBaseE },
    { internal::kLogBase2Str, kLogBase2 },
    { internal::kLogBase10Str, kLogBase10 }
  };
  auto lbit = map.find(value);
  if (lbit == map.end()) {
    throw InvalidParameterValueException();
  }
  return lbit->second;
}

void LogRaw::Do(bool simd, const float* input, int length,
                float* output) const noexcept {
  switch (base()) {
    case kLogBaseE: {
      if (simd) {
#ifdef __AVX__
        for (int j = 0; j < length - 7; j += 8) {
          __m256 vec = _mm256_load_ps(input + j);
          vec = log256_ps(vec);
          _mm256_store_ps(output + j, vec);
        }
        for (int j = ((length >> 3) << 3); j < length; j++) {
          output[j] = logf(input[j]);
        }
      } else {
#elif defined(__ARM_NEON__)
        int length = input_format_->Size();
        for (int j = 0; j < length - 3; j += 4) {
          float32x4_t vec = vld1q_f32(input + j);
          vec = log_ps(vec);
          vst1q_f32(output + j, vec);
        }
        for (int j = ((length >> 2) << 2); j < length; j++) {
          output[j] = logf(input[j]);
        }
      } else {
#else
      } {
#endif
        for (int j = 0; j < length; j++) {
          output[j] = logf(input[j]);
        }
      }
      break;
    }
    case kLogBase2:
      for (int j = 0; j < length; j++) {
        output[j] = log2f(input[j]);
      }
      break;
    case kLogBase10:
      for (int j = 0; j < length; j++) {
        output[j] = log10f(input[j]);
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
  switch (base()) {
    case kLogBaseE:
      *out = logf(in);
      break;
    case kLogBase2:
      *out = log2f(in);
      break;
    case kLogBase10:
      *out = log10f(in);
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
