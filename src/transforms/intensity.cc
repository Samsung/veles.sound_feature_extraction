/*! @file intensity.cc
 *  @brief Sound intensity calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/intensity.h"
#include <math.h>
#include <string>
#include "src/primitives/arithmetic-inl.h"
#ifdef __AVX__
#include "src/primitives/avx_extra.h"
#endif

namespace SpeechFeatureExtraction {
namespace Transforms {

Intensity::Intensity()
  :TransformBase(SupportedParameters()) {
}

void Intensity::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<float>* buffers) const noexcept {
  buffers->Initialize(in.Size());
}

void Intensity::TypeSafeDo(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<float> *out) const noexcept {
  int length = inputFormat_->Size();
  for (size_t i = 0; i < in.Size(); i++) {
    auto input = in[i]->Data.get();
    double intensity =.0f;
#ifdef __AVX__
    for (int j = 0; j < length - 7; j += 8) {
      __m256 vec = _mm256_load_ps(input + j);
      vec = _mm256_mul_ps(vec, vec);
      __m256 vecp = _mm256_permute2f128_ps(vec, vec, 1);
      vec = _mm256_add_ps(vec, vecp);
      vec = _mm256_hadd_ps(vec, vec);
      intensity += ElementAt(vec, 0) + ElementAt(vec, 1);
    }
    for (int j = ((length >> 3) << 3); j < length; j++) {
      intensity += input[j] * input[j];
    }
#elif defined(__ARM_NEON__)
    for (int j = 0; j < length - 3; j += 4) {
      float32x4_t vec = vld1q_f32(input + j);
      vec = vmulq_f32(vec, vec);
      float32x2_t vec2 = vadd_f32(vget_high_f32(vec), vget_low_f32(vec));
      float32x2_t res = vpadd_f32(vec2, vec2);
      intensity += vget_lane_f32(res, 0);
    }
    for (int j = ((length >> 2) << 2); j < length; j++) {
      intensity += input[j] * input[j];
    }
#else
    for (int j = 0; j < length; j++) {
      intensity += input[j] * input[j];
    }
#endif
    *(*out)[i] = logf(intensity / length);
  }
}

REGISTER_TRANSFORM(Intensity);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
