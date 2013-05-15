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
#include <math.h>
#ifdef __AVX__
#include "src/primitives/avx_mathfun.h"
#elif defined(__ARM_NEON__)
#include "src/primitives/neon_mathfun.h"
#endif

namespace SoundFeatureExtraction {
namespace Transforms {

const std::unordered_map<std::string, Log::LogBase> Log::kLogBaseMap {
  { "e", LOG_BASE_E },
  { "2", LOG_BASE_2 },
  { "10", LOG_BASE_10 }
};

const Log::LogBase Log::kDefaultLogBase = LOG_BASE_E;

Log::Log() : base_(kDefaultLogBase) {
  RegisterSetter("base", [&](const std::string& value) {
    auto lbit = kLogBaseMap.find(value);
    if (lbit == kLogBaseMap.end()) {
      return false;
    }
    base_ = lbit->second;
    return true;
  });
}

void Log::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size());
}

void Log::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
  assert(!IsInverse() && "Not implemented yet");
  for (size_t i = 0; i < in.Size(); i++) {
    Do(true, in[i]->Data.get(), inputFormat_->Size(), (*out)[i]->Data.get());
  }
}

void Log::Do(bool simd, const float* input, int length,
             float* output) const noexcept {
  switch (base_) {
    case LOG_BASE_E: {
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
        int length = inputFormat_->Size();
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
        for (size_t j = 0; j < inputFormat_->Size(); j++) {
          output[j] = logf(input[j]);
        }
      }
      break;
    }
    case LOG_BASE_2:
      for (int j = 0; j < length; j++) {
        output[j] = log2f(input[j]);
      }
      break;
    case LOG_BASE_10:
      for (int j = 0; j < length; j++) {
        output[j] = log10f(input[j]);
      }
      break;
  }
}

std::string Log::LogBaseToString(LogBase lb) noexcept {
  switch (lb) {
    case LOG_BASE_E:
      return "e";
    case LOG_BASE_2:
      return "2";
    case LOG_BASE_10:
      return "10";
  }
  return "";
}

REGISTER_TRANSFORM(Log);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
