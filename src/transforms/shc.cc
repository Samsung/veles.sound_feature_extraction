/*! @file shc.cc
 *  @brief Prepare for fundamental frequency extraction using Spectral Harmonics Correlation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/shc.h"
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include <simd/normalize.h>
#include <simd/arithmetic-inl.h>

namespace SoundFeatureExtraction {
namespace Transforms {

SHC::SHC()
    : harmonics_(kDefaultHarmonicsNumber),
      window_width_(kDefaultWindowWidth),
      min_freq_(kDefaultMinFrequency),
      max_freq_(kDefaultMaxFrequency),
      half_window_samples_(0),
      min_samples_(0),
      max_samples_(0) {
  RegisterSetter("length", [&](const std::string& value) {
    int pv = Parse<int>("length", value);
    if (pv < 1) {
      return false;
    }
    harmonics_ = pv;
    return true;
  });
  RegisterSetter("window", [&](const std::string& value) {
    int pv = Parse<int>("window", value);
    if (pv < 0) {
      return false;
    }
    window_width_ = pv;
    return true;
  });
  RegisterSetter("min", [&](const std::string& value) {
    int pv = Parse<int>("min", value);
    if (pv < 1) {
      return false;
    }
    min_freq_ = pv;
    return true;
  });
  RegisterSetter("max", [&](const std::string& value) {
    int pv = Parse<int>("max", value);
    if (pv < 1) {
      return false;
    }
    max_freq_ = pv;
    return true;
  });
}

void SHC::Initialize() const {
  half_window_samples_ = inputFormat_->Size() * window_width_ /
    inputFormat_->SamplingRate();
}

size_t SHC::OnFormatChanged(size_t buffersCount) {
  if (max_freq_ <= min_freq_ ||
      min_freq_ - window_width_ / 2 < 0 ||
      max_freq_ + window_width_ / 2 >= inputFormat_->SamplingRate() / 2) {
    throw SHCInvalidMinMaxWindowWidthException(min_freq_, max_freq_,
                                               window_width_,
                                               inputFormat_->SamplingRate());
  }
  min_samples_ = inputFormat_->Size() * 2 * min_freq_ /
      inputFormat_->SamplingRate();
  max_samples_ = inputFormat_->Size() * 2 * max_freq_ /
      inputFormat_->SamplingRate();
  outputFormat_->SetSize(max_samples_ - min_samples_ + 1);
  return buffersCount;
}

void SHC::Do(const float* in, float* out) const noexcept {
  for (int i = min_samples_; i <= max_samples_; i++) {
    float sum = 0;
    if (UseSimd()) {
#ifdef __AVX__
      __m256 sum_vec = _mm256_set1_ps(0);
      int f;
      for (f = 0; f <= half_window_samples_ * 2 - 7;
          f += 8) {
        __m256 prod = _mm256_set1_ps(1);
        for (int j = 0; j < harmonics_; j++) {
          __m256 vec = _mm256_loadu_ps(
              in + i * (j + 1) + f - half_window_samples_);
          prod = _mm256_mul_ps(prod, vec);
        }
        sum_vec = _mm256_add_ps(sum_vec, prod);
      }
      for (; f <= half_window_samples_ * 2; f++) {
        float prod = 1;
        for (int j = 0; j < harmonics_; j++) {
          prod *= in[i * (j + 1) + f - half_window_samples_];
        }
        sum += prod;
      }
      sum_vec = _mm256_hadd_ps(sum_vec, sum_vec);
      sum_vec = _mm256_hadd_ps(sum_vec, sum_vec);
      sum += ElementAt(sum_vec, 0) + ElementAt(sum_vec, 4);
    } else {
#elif defined(__ARM_NEON__)
      float32x4_t sum_vec = vdupq_n_f32(0);
      int f;
      for (f = 0; f <= half_window_samples_ * 2 - 3;
          f += 4) {
        float32x4_t prod = vdupq_n_f32(1);
        for (int j = 0; j < harmonics_; j++) {
          float32x4_t vec = vld1q_f32(
              in + i * (j + 1) + f - half_window_samples_);
          prod = vmulq_f32(prod, vec);
        }
        sum_vec = vaddq_f32(sum_vec, prod);
      }
      for (; f <= half_window_samples_ * 2; f++) {
        float prod = 1;
        for (int j = 0; j < harmonics_; j++) {
          prod *= in[i * (j + 1) + f - half_window_samples_];
        }
        sum += prod;
      }
      sum += vgetq_lane_f32(sum_vec, 0) + vgetq_lane_f32(sum_vec, 1) +
          vgetq_lane_f32(sum_vec, 2) + vgetq_lane_f32(sum_vec, 3);
    } else {
#else
    } {
#endif
      for (int f = -half_window_samples_; f <= half_window_samples_; f++) {
        float prod = 1;
        for (int j = 0; j < harmonics_; j++) {
          prod *= in[i * (j + 1) + f];
        }
        sum += prod;
      }
    }
    out[i - min_samples_] = sum;
  }
  float max;
  minmax1D(UseSimd(), out, outputFormat_->Size(), nullptr, &max);
  if (UseSimd()) {
    real_multiply_scalar(out, outputFormat_->Size(), 1 / max, out);
  } else {
    real_multiply_scalar_na(out, outputFormat_->Size(), 1 / max, out);
  }
}

REGISTER_TRANSFORM(SHC);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
