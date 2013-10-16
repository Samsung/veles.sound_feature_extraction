/*! @file mix_stereo.cc
 *  @brief Mix audio channels together.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/mix_stereo.h"
#ifdef __AVX__
#include <immintrin.h>  // NOLINT(build/include_order)
#elif defined(__ARM_NEON__)
#include <arm_neon.h>  // NOLINT(build/include_order)
#endif

namespace SoundFeatureExtraction {
namespace Transforms {

size_t MixStereo::OnFormatChanged(size_t buffersCount) {
  outputFormat_->SetSize(inputFormat_->Size() / 2);
  return buffersCount;
}

void MixStereo::Do(const int16_t* in, int16_t* out) const noexcept {
  int length = inputFormat_->Size();
  if (UseSimd()) {
#ifdef __AVX__
    for (int i = 0; i < length - 15; i += 16) {
      __m128i vec1 = _mm_load_si128(
          reinterpret_cast<const __m128i*>(in + i));
      __m128i vec2 = _mm_load_si128(
          reinterpret_cast<const __m128i*>(in + i + 8));
      vec1 = _mm_srai_epi16(vec1, 1);
      vec2 = _mm_srai_epi16(vec2, 1);
      __m128i res = _mm_hadd_epi16(vec1, vec2);
      _mm_store_si128(reinterpret_cast<__m128i*>(out + i / 2), res);
    }
    for (int i = (length & ~0xF); i < length; i += 2) {
      int16_t l = in[i] / 2;
      int16_t r = in[i + 1] / 2;
      out[i / 2] = l + r;
    }
  } else {
#elif defined(__ARM_NEON__)
    for (int i = 0; i < length - 15; i += 16) {
      int16x8x2_t vec = vld2q_s16(input + i);
      int16x8_t res = vshrq_n_s16(vec.val[0], 1);
      res = vsraq(res, vec.val[1], 1);
      vst1q_s16(out + i / 2, res);
    }
    for (int i = (length & ~0xF); i < length; i += 2) {
      int16_t l = in[i] / 2;
      int16_t r = in[i + 1] / 2;
      out[i / 2] = l + r;
    }
  } else {
#else
  } {
#endif
    for (int i = 0; i < length; i += 2) {
      int16_t l = in[i] / 2;
      int16_t r = in[i + 1] / 2;
      out[i / 2] = l + r;
    }
  }
}


REGISTER_TRANSFORM(MixStereo);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
