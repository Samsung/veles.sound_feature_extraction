/*! @file preemphasis.cc
 *  @brief Number of time domain zero crossings of the signal.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/preemphasis.h"
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include <simd/avx_extra.h>

namespace SoundFeatureExtraction {
namespace Transforms {

const float Preemphasis::kDefaultK = 0.9f;

Preemphasis::Preemphasis()
  : k_(kDefaultK) {
  RegisterSetter("value", [&](const std::string& value) {
    auto pv = Parse<float>("value", value);
    if (pv > 1 || pv <= 0) {
      return false;
    }
    k_ = pv;
    return true;
  });
}

void Preemphasis::InitializeBuffers(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Raw16>* buffers) const noexcept {
  buffers->Initialize(in.Size(), outputFormat_->Size(),
                      in[0].AlignmentOffset());
}

void Preemphasis::Do(const Formats::Raw16& in,
                     Formats::Raw16* out) const noexcept {

  Do(UseSimd(), in.Data.get(), inputFormat_->Size(), k_, out->Data.get());
}

void Preemphasis::Do(bool simd, const int16_t* input, size_t length,
                     float k, int16_t* output)
    noexcept {
  int ilength = length;
  output[0] = input[0];
  if (simd) {
#ifdef __AVX__
#ifdef __AVX2__
#error Use AVX2 here instead of SSE
#endif
    const __m128 veck = _mm_set1_ps(k);
    for (int i = 1; i < ilength - 8; i += 8) {
      __m128i vecpre = _mm_loadu_si128(
          reinterpret_cast<const __m128i*>(input + i - 1));
      __m128i intlo = _mm_unpacklo_epi16(_mm_set1_epi16(0), vecpre);
      __m128i inthi = _mm_unpackhi_epi16(_mm_set1_epi16(0), vecpre);
      intlo = _mm_sra_epi32(intlo, _mm_set1_epi32(16));
      inthi = _mm_sra_epi32(inthi, _mm_set1_epi32(16));
      __m128 flo = _mm_cvtepi32_ps(intlo);
      __m128 fhi = _mm_cvtepi32_ps(inthi);
      flo = _mm_mul_ps(flo, veck);
      fhi = _mm_mul_ps(fhi, veck);
      intlo = _mm_cvttps_epi32(flo);
      inthi = _mm_cvttps_epi32(fhi);
      vecpre = _mm_packs_epi32(intlo, inthi);
      __m128i vec = _mm_loadu_si128(
          reinterpret_cast<const __m128i*>(input + i));
      vec = _mm_subs_epi16(vec, vecpre);
      _mm_storeu_si128(reinterpret_cast<__m128i*>(output + i), vec);
    }
    for (int i = ((ilength - 1) & ~7); i < ilength; i++) {
      output[i] = input[i] - k * input[i - 1];
    }
    return;
  } else {
#elif defined(__ARM_NEON__)
    for (int i = 1; i < ilength - 8; i += 8) {
      int16x8_t vecpre = vld1q_s16(input + i - 1);
      int16x8_t vec = vld1q_s16(input + i);
      vec = vqsubq_s16(vec, vecpre);
      vst1q_s16(output + i, vec);
    }
    for (int i = ((ilength - 1) & ~7); i < ilength; i++) {
      output[i] = input[i] - k * input[i - 1];
    }
    return;
  } else {
#else
  } {
#endif
    for (int i = 1; i < ilength; i++) {
      output[i] = input[i] - k * input[i - 1];
    }
  }
}

REGISTER_TRANSFORM(Preemphasis);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
