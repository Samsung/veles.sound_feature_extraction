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

#define __STDC_LIMIT_MACROS
#include "src/transforms/diffrect.h"
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

namespace SoundFeatureExtraction {
namespace Transforms {

void Diffrect::OnFormatChanged() {
  outputFormat_->SetSize(inputFormat_->Size() - 1);
}

void Diffrect::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), outputFormat_->Size());
}

void Diffrect::Do(const Formats::WindowF& in, Formats::WindowF* out)
const noexcept {
  Do(true, in.Data.get(), inputFormat_->Size(), out->Data.get());
}

void Diffrect::Do(bool simd, const float* input, int length,
                  float* output) noexcept {
  if (simd) {
#ifdef __AVX__
    int first = 0;
    for (int i = 0; i < length - 15; i += 8) {
      __m256 vec1 = _mm256_load_ps(input + i);
      __m256 vec2 = _mm256_load_ps(input + i + 8);
      vec2 = _mm256_blend_ps(vec1, vec2, 1);
      vec1 = _mm256_shuffle_ps(vec1, vec1, 1 << 0 | 2 << 4 | 1 << 6);
      vec2 = _mm256_shuffle_ps(vec2, vec2, 3 << 0 | 2 << 2 | 3 << 6);
      __m256 vec3 = _mm256_permute2f128_ps(vec2, vec2, 1);
      vec2 = _mm256_blend_ps(vec2, vec3, 68);
      __m256 result = _mm256_hsub_ps(vec1, vec2);
      result = _mm256_max_ps(result, _mm256_setzero_ps());
      _mm256_store_ps(output + i, result);
      first += 8;
    }
    for (int i = first; i < length - 1; i++) {
      output[i] = input[i + 1] - input[i];
      if (output[i] < 0) {
        output[i] = 0;
      }
    }
  } else {
#elif defined(__ARM_NEON__) //TODO (A.Shapichev) : ARM_NEON implementation of differentiation
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

REGISTER_TRANSFORM(Diffrect);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
