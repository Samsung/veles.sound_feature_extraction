/*! @file subband_energy.cc
 *  @brief Calculate the subband energy.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/subband_energy.h"
#include <math.h>
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include "src/primitives/wavelet_filter_bank.h"

namespace SpeechFeatureExtraction {

using Primitives::WaveletFilterBank;

namespace Transforms {

SubbandEnergy::SubbandEnergy()
: UniformFormatTransform(SupportedParameters()),
  treeFingerprint_({ 3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                     6, 6, 6, 6, 6, 6, 6, 6 }) {
}

void SubbandEnergy::SetParameter(const std::string& name,
                                 const std::string& value) {
  if (name == "tree") {
    treeFingerprint_ = WaveletFilterBank::ParseDescription(value);
  }
}

void SubbandEnergy::OnInputFormatChanged() {
  outputFormat_->SetDuration(inputFormat_->Duration());
  outputFormat_->SetSamplingRate(inputFormat_->SamplingRate());
  WaveletFilterBank::ValidateLength(treeFingerprint_,
                                    inputFormat_->Size());
  outputFormat_->SetSize(treeFingerprint_.size());
}

void SubbandEnergy::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), treeFingerprint_.size());
}

void SubbandEnergy::TypeSafeDo(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    auto input = in[i]->Data.get();
    auto output = (*out)[i]->Data.get();
    int length = inputFormat_->Size();
#ifdef __AVX__
    for (int j = 0; j < length - 15; j += 16) {
      __m256 vec1 = _mm256_load_ps(input + j);
      __m256 vec2 = _mm256_load_ps(input + j + 8);
      vec1 = _mm256_mul_ps(vec1, vec1);
      vec2 = _mm256_mul_ps(vec2, vec2);
      __m256 r1 = _mm256_permute2f128_ps(vec1, vec2, 0x20);
      __m256 r2 = _mm256_permute2f128_ps(vec1, vec2, 0x31);
      __m256 res = _mm256_hadd_ps(r1, r2);
      _mm256_store_ps(output + j / 2, res);
    }
    for (int i = ((length >> 4) << 4); i < length; i += 2) {
      float re = input[i];
      float im = input[i + 1];
      output[i / 2] = re * re + im * im;
    }
#elif defined(__ARM_NEON__)
    for (int j = 0; j < length - 3; j += 4) {
      float32x4_t cvec = vld1q_f32(input + j);
      float32x4_t sqrvec = vmulq_f32(cvec, cvec);
      float32x2_t sums = vpadd_f32(vget_high_f32(sqrvec),
                                   vget_low_f32(sqrvec));
      vst1_f32(sums, output + j / 2);
    }
    for (int j = ((length >> 2) << 2); j < length; j += 2) {
      float re = input[j];
      float im = input[j + 1];
      output[j / 2] = re * re + im * im;
    }
#else
    for (int i = 0; i < length; i += 2) {
      float re = input[i];
      float im = input[i + 1];
      output[i / 2] = sqrtf(re * re + im * im);
    }
#endif
  }
}

REGISTER_TRANSFORM(SubbandEnergy);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
