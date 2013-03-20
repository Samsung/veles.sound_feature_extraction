/*! @file real_to_complex.cc
 *  @brief Converts real numbers to the corresponding complex numbers.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#define __STDC_LIMIT_MACROS
#include "src/transforms/real_to_complex.h"
#include <string>
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

namespace SpeechFeatureExtraction {
namespace Transforms {

void RealToComplex::OnFormatChanged() {
  outputFormat_->SetSize(inputFormat_->Size() * 2);
}

void RealToComplex::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), outputFormat_->Size());
}

void RealToComplex::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    auto input = in[i]->Data.get();
    auto output = (*out)[i]->Data.get();
    int length = inputFormat_->Size();
#ifdef __AVX__
#ifndef __AVX2__
    const __m128 zeros = _mm_set_ps(.0f, .0f, .0f, .0f);
    for (int i = 0; i < length - 3; i += 4) {
      __m128 vec = _mm_load_ps(input + i);
      __m128 low = _mm_unpacklo_ps(vec, zeros);
      __m128 high = _mm_unpackhi_ps(vec, zeros);
      _mm_store_ps(output + 2 * i, low);
      _mm_store_ps(output + 2 * i + 4, high);
    }
    for (int i = ((length >> 2) << 2); i < length; i++) {
      output[i * 2] = input[i];
      output[i * 2 + 1] = .0f;
    }
#else
#error TODO: AVX2 introduces a full 256-bit permute which must be executed before _mm256_unpack*
#endif
#elif defined(__ARM_NEON__)
    const float32x4_t zeros = { .0f, .0f, .0f, .0f };
    for (int i = 0; i < length - 3; i += 4) {
      float32x4_t vec = vld1q_f32(input + i);
      float32x4x2_t res = vzipq_f32(vec, zeros);
      vst2q_f32(output + i * 2, res);
    }

    for (int i = ((length >> 2) << 2); i < length; i++) {
      output[i * 2] = input[i];
      output[i * 2 + 1] = .0f;
    }
#else
    for (int i = 0; i < length; i++) {
      output[i * 2] = input[i];
      output[i * 2 + 1] = .0f;
    }
#endif
  }
}

REGISTER_TRANSFORM(RealToComplex);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
