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

namespace SoundFeatureExtraction {
namespace Transforms {

void RectifyWindow::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size());
}

void RectifyRaw::InitializeBuffers(
    const BuffersBase<Formats::RawF>& in,
    BuffersBase<Formats::RawF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), outputFormat_->Size(),
                      in[0].AlignmentOffset());
}

void RectifyBase::Do(bool simd, const float* input, int length,
                     float* output) noexcept {
  if (simd) {
#ifdef __AVX__
    const __m256 SIGNMASK =  _mm256_set1_ps(-0.f);
    // Unroll 1 time
    for (int i = 0; i < length - 15; i += 16) {
      __m256 vec1 = _mm256_load_ps(input + i);
      __m256 vec2 = _mm256_load_ps(input + i + 8);
      __m256 vecabs1 = _mm256_andnot_ps(SIGNMASK, vec1);
      __m256 vecabs2 = _mm256_andnot_ps(SIGNMASK, vec2);
      _mm256_store_ps(output + i, vecabs1);
      _mm256_store_ps(output + i + 8, vecabs2);
    }
    for (int i = ((length >> 4) << 4); i < length; i++) {
      output[i] = abs(input[i]);
    }
  } else {
#elif defined(__ARM_NEON__)
    for (int i = 0; i < length - 7; i += 8) {
      float32x4_t vec1 = vld1q_f32(input + i);
      float32x4_t vec2 = vld1q_f32(input + i + 4);
      float32x4_t res1 = vabsq_f32(vec1);
      float32x4_t res2 = vabsq_f32(vec2);
      vst1q_f32(output + i, res1);
      vst1q_f32(output + i + 4, res2);
    }
    for (int i = ((length >> 3) << 3); i < length; i += 2) {
      output[i / 2] = input[i];
    }
  } else {
#else
  } {
#endif
    for (int i = 0; i < length; i++) {
      output[i] = abs(input[i]);
    }
  }
}

REGISTER_TRANSFORM(RectifyWindow);
REGISTER_TRANSFORM(RectifyRaw);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
