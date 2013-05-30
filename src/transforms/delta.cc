/*! @file delta.cc
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/delta.h"
#include <simd/arithmetic-inl.h>

namespace SoundFeatureExtraction {
namespace Transforms {

void Delta::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size() - 1, inputFormat_->Size());
}

void Delta::Do(const BuffersBase<Formats::WindowF>& in,
               BuffersBase<Formats::WindowF>* out) const noexcept {
  for (size_t i = 1; i < in.Size(); i++) {
    Do(true, in[i - 1].Data.get(), in[i].Data.get(),
       inputFormat_->Size(), (*out)[i - 1].Data.get());
  }
}

void Delta::Do(bool simd, const float* prev, const float* cur,
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
    for (int i = ilength & ~7; i < ilength; i++) {
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
    for (int i = ilength & ~3; i < ilength; i++) {
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

REGISTER_TRANSFORM(Delta);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
