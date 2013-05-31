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

const std::unordered_map<std::string, Delta::Type> Delta::kTypesMap = {
    { "simple", kTypeSimple },
    { "regression", kTypeRegression }
};

Delta::Delta()
  : type_(kTypeSimple),
    rlength_(kDefaultRegressionLength) {
  RegisterSetter("type", [&](const std::string& value) {
    auto pvi = kTypesMap.find(value);
    if (pvi == kTypesMap.end()) {
      return false;
    }
    type_ = pvi->second;
    return true;
  });
  RegisterSetter("rlength", [&](const std::string& value) {
    int pv = Parse<int>("rlength", value);
    if (pv < 3 || pv % 2 == 0) {
      return false;
    }
    rlength_ = pv;
    return true;
  });
}

void Delta::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size());
}

void Delta::Do(const BuffersBase<Formats::WindowF>& in,
               BuffersBase<Formats::WindowF>* out) const noexcept {

  switch (type_) {
    case kTypeSimple:
      for (size_t i = 1; i < in.Size(); i++) {
        DoSimple(true, in[i - 1].Data.get(), in[i].Data.get(),
                 inputFormat_->Size(), (*out)[i].Data.get());
      }
      for (size_t i = 0; i < inputFormat_->Size(); i++) {
        (*out)[0][i] = (*out)[1][i];
      }
    break;
    case kTypeRegression: {
      int rstep = rlength_ / 2;
      float norm = 2 * rstep * (rstep + 1) * (2 * rstep + 1) / 6;
      for (size_t i = rstep; i < in.Size() - rstep; i++) {
        DoRegression(true, in, rstep, i, norm, inputFormat_->Size(), out);
      }
      for (size_t i = rstep - 1; i > 0; i--) {
        norm = 2 * i * (i + 1) * (2 * i + 1) / 6;
        DoRegression(true, in, i, i, norm, inputFormat_->Size(), out);
        DoRegression(true, in, in.Size() - i, i, norm, inputFormat_->Size(),
                     out);
      }
      for (size_t i = 0; i < inputFormat_->Size(); i++) {
        (*out)[0][i] = (*out)[1][i];
        (*out)[in.Size() - 1][i] = (*out)[in.Size() - 2][i];
      }
      break;
    }
  }
}

void Delta::DoSimple(bool simd, const float* prev, const float* cur,
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

void Delta::DoRegression(bool simd, const BuffersBase<Formats::WindowF>& in,
                         int rstep, int i, float norm, int windowSize,
                         BuffersBase<Formats::WindowF>* out) noexcept {
  if (simd) {
#ifdef __AVX__
    __m256 normvec = _mm256_set1_ps(norm);
    for (int j = 0; j < windowSize - 7; j += 8) {
      __m256 sum = _mm256_setzero_ps();
      __m256 kvec = _mm256_set1_ps(1.f);
      for (int k = 1; k <= rstep; k++) {
        __m256 rvec = _mm256_load_ps(in[i + k].Data.get() + j);
        __m256 lvec = _mm256_load_ps(in[i - k].Data.get() + j);
        __m256 diff = _mm256_sub_ps(rvec, lvec);
        diff = _mm256_mul_ps(diff, kvec);
        sum = _mm256_add_ps(sum, diff);
        kvec = _mm256_add_ps(kvec, _mm256_set1_ps(1.f));
      }
      sum = _mm256_div_ps(sum, normvec);
      _mm256_store_ps((*out)[i].Data.get() + j, sum);
    }
    for (int j = windowSize & ~7; j < windowSize; j++) {
      float sum = 0.f;
      for (int k = 1; k <= rstep; k++) {
        sum += (in[i + k][j] - in[i - k][j]) * k;
      }
      (*out)[i][j] = sum / norm;
    }
    return;
  } else {
#elif defined(__ARM_NEON__)
    float32x4_t normvec = vdupq_n_f32(1.f / norm);
    for (int j = 0; j < windowSize - 3; j += 4) {
      float32x4_t sum = vdupq_n_f32(0.f);
      float32x4_t kvec = vdupq_n_f32(1.f);
      for (int k = 1; k <= rstep; k++) {
        float32x4_t rvec = vld1q_f32(in[i + k].Data.get() + j);
        float32x4_t lvec = vld1q_f32(in[i - k].Data.get() + j);
        float32x4_t diff = vsubq_f32(rvec, lvec);
        sum = vmlaq_f32(diff, kvec, sum);
        kvec = vaddq_f32(kvec, vdupq_n_f32(1.f));
      }
      sum = vmulq_f32(sum, normvec);
      vst1q_f32((*out)[i].Data.get() + j, sum);
    }
    for (int j = windowSize & ~3; j < windowSize; j++) {
      float sum = 0.f;
      for (int k = 1; k <= rstep; k++) {
        sum += (in[i + k][j] - in[i - k][j]) * k;
      }
      (*out)[i][j] = sum / norm;
    }
    return;
  } else {
#else
  } {
#endif
    for (int j = 0; j < windowSize; j++) {
      float sum = 0.f;
      for (int k = 1; k <= rstep; k++) {
        sum += (in[i + k][j] - in[i - k][j]) * k;
      }
      (*out)[i][j] = sum / norm;
    }
  }
}

REGISTER_TRANSFORM(Delta);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
