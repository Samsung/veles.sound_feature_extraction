/*! @file rectify.cc
 *  @brief Wave rectification.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/rectify.h"
#ifdef __AVX__
#include <immintrin.h>  // NOLINT(build/include_order)
#elif defined(__ARM_NEON__)
#include <arm_neon.h>  // NOLINT(build/include_order)
#endif

namespace SpeechFeatureExtraction {
namespace Transforms {

void Rectify::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size());
}

void Rectify::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    Do(true, in[i]->Data.get(), inputFormat_->Size(), (*out)[i]->Data.get());
  }
}

void Rectify::Do(bool simd, const float* input, int length,
                 float* output) noexcept {
  if (simd) {
#ifdef __AVX__
    for (int i = 0; i < length - 7; i += 8) {
      __m256 vec1 = _mm256_load_ps(input + i);
      __m256 vec2 = _mm256_sub_ps(_mm256_setzero_ps(), vec1);
      vec1 = _mm256_max_ps(vec1, vec2);
      _mm256_store_ps(output + i, vec1);
    }
    for (int i = ((length >> 3) << 3); i < length; i++) {
      output[i] = abs(input[i]);
    }
  } else {
#elif defined(__ARM_NEON__)
    // TODO(a.shapichev) : ARM_NEON implementation of rectification
  } else {
#else
  } {
#endif
    for (int i = 0; i < length; i++) {
      output[i] = abs(input[i]);
    }
  }
}

REGISTER_TRANSFORM(Rectify);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
