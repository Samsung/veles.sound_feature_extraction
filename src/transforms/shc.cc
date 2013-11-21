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
#include <simd/normalize.h>
#include <simd/arithmetic-inl.h>

namespace sound_feature_extraction {
namespace transforms {

SHC::SHC()
    : harmonics_(kDefaultHarmonicsNumber),
      window_(kDefaultWindowWidth),
      min_(kDefaultMinFrequency),
      max_(kDefaultMaxFrequency),
      half_window_samples_(0),
      min_samples_(0),
      max_samples_(0) {
}


bool SHC::validate_harmonics(const int& value) noexcept {
  return value >= 1;
}

bool SHC::validate_window(const int& value) noexcept {
  return value >= 0;
}

bool SHC::validate_min(const int& value) noexcept {
  return value >= 1;
}

bool SHC::validate_max(const int& value) noexcept {
  return value >= 1;
}

void SHC::Initialize() const {
  half_window_samples_ = input_format_->Size() * window_ /
    input_format_->SamplingRate();
}

size_t SHC::OnFormatChanged(size_t buffersCount) {
  if (max_ <= min_ ||
      min_ - window_ / 2 < 0 ||
      max_ + window_ / 2 >= input_format_->SamplingRate() / 2) {
    throw SHCInvalidMinMaxWindowWidthException(min_, max_,
                                               window_,
                                               input_format_->SamplingRate());
  }
  min_samples_ = input_format_->Size() * 2 * min_ /
      input_format_->SamplingRate();
  max_samples_ = input_format_->Size() * 2 * max_ /
      input_format_->SamplingRate();
  output_format_->SetSize(max_samples_ - min_samples_ + 1);
  return buffersCount;
}

void SHC::Do(const float* in, float* out) const noexcept {
  for (int i = min_samples_; i <= max_samples_; i++) {
    float sum = 0;
    if (use_simd()) {
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
      sum += _mm256_get_ps(sum_vec, 0) + _mm256_get_ps(sum_vec, 4);
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
  minmax1D(use_simd(), out, output_format_->Size(), nullptr, &max);
  if (use_simd()) {
    real_multiply_scalar(out, output_format_->Size(), 1 / max, out);
  } else {
    real_multiply_scalar_na(out, output_format_->Size(), 1 / max, out);
  }
}

RTP(SHC, harmonics)
RTP(SHC, window)
RTP(SHC, min)
RTP(SHC, max)
REGISTER_TRANSFORM(SHC);

}  // namespace transforms
}  // namespace sound_feature_extraction
