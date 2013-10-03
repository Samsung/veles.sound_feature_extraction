/*! @file diffrect.cc
 *  @brief Find the difference from one sample to the next.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/diff.h"
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

namespace SoundFeatureExtraction {
namespace Transforms {

Diff::Diff() : rectify_(false) {
  RegisterSetter("rectify", [&](const std::string& value) {
    rectify_ = Parse<bool>("rectify", value);
    return true;
  });
}

size_t Diff::OnFormatChanged(size_t buffersCount) {
  outputFormat_->SetSize(inputFormat_->Size() - 1);
  return buffersCount;
}

void Diff::Do(const float* in, float* out) const noexcept {
  if (rectify_) {
    DoRectify(UseSimd(), in, inputFormat_->Size(), out);
  } else {
    Do(UseSimd(), in, inputFormat_->Size(), out);
  }
}

void Diff::Do(bool simd, const float* input, int length,
              float* output) noexcept {
  if (simd) {
#ifdef __AVX__
    for (int i = 1; i < length - 7; i += 8) {
      __m256 vec1 = _mm256_loadu_ps(input + i);
      __m256 vec2 = _mm256_load_ps(input + i - 1);
      __m256 result = _mm256_sub_ps(vec1, vec2);
      _mm256_store_ps(output + i - 1, result);
    }
    for (int i = (length & ~0x7); i < length - 1; i++) {
      output[i] = input[i + 1] - input[i];
    }
  } else {
#elif defined(__ARM_NEON__)
    for (int i = 1; i < length - 3; i += 4) {
      float32x4_t vec1 = vld1q_f32(input + i);
      float32x4_t vec2 = vld1q_f32(input + i - 1);
      float32x4_t result = vsubq_f32(vec1, vec2);
      vst1q_f32(output + i - 1, result);
    }
    for (int i = (length & ~0x3); i < length - 1; i++) {
      output[i] = input[i + 1] - input[i];
    }
  } else {
#else
  } {
#endif
    for (int i = 0; i < length - 1; i++) {
      output[i] = input[i + 1] - input[i];
    }
  }
}

void Diff::DoRectify(bool simd, const float* input, int length,
                     float* output) noexcept {
  if (simd) {
#ifdef __AVX__
    for (int i = 0; i < length - 15; i += 8) {
      for (int i = 1; i < length - 7; i += 8) {
        __m256 vec1 = _mm256_loadu_ps(input + i);
        __m256 vec2 = _mm256_load_ps(input + i - 1);
        __m256 result = _mm256_sub_ps(vec1, vec2);
        result = _mm256_max_ps(result, _mm256_setzero_ps());
        _mm256_store_ps(output + i - 1, result);
      }
    }
    for (int i = (length & ~0x7); i < length - 1; i++) {
      output[i] = input[i + 1] - input[i];
      if (output[i] < 0) {
        output[i] = 0;
      }
    }
  } else {
#elif defined(__ARM_NEON__)
    for (int i = 1; i < length - 3; i += 4) {
      float32x4_t vec1 = vld1q_f32(input + i);
      float32x4_t vec2 = vld1q_f32(input + i - 1);
      float32x4_t result = vsubq_f32(vec1, vec2);
      result = vmaxq_f32(result, vdupq_n_f32(0));
      vst1q_f32(output + i - 1, result);
    }
    for (int i = (length & ~0x3); i < length - 1; i++) {
      output[i] = input[i + 1] - input[i];
    }
  } else {
#else
  } {
#endif
    for (int i = 0; i < length - 1; i++) {
      output[i] = input[i + 1] - input[i];
      if (output[i] < 0) {
        output[i] = 0;
      }
    }
  }
}

REGISTER_TRANSFORM(Diff);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
