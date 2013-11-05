/*! @file argminmax.cc
 *  @brief Arithmetic and geometric argminmaxs calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/argminmax.h"
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include <simd/avx_extra.h>

namespace sound_feature_extraction {
namespace transforms {

const bool ArgMinMax::kDefaultExtremum = true;

ArgMinMax::ArgMinMax() : extremum_(kDefaultExtremum) {
  RegisterSetter("extremum", [&](const std::string& value) {
    if (value == "min") {
      extremum_ = true;
    } else if(value == "max") {
      extremum_ = false;
    } else {
      return false;
    }
    return true;
  });
}

void ArgMinMax::Do(const float* in,
                   ArgMinMaxResult *out) const noexcept {
  *out = Do(UseSimd(), in, input_format_->Size(), extremum_);
}

ArgMinMaxResult ArgMinMax::Do(bool simd, const float* input, size_t length,
                              bool min) noexcept {
  int ilength = static_cast<int>(length);
  int index;
  float extr;
  if (simd) {
#ifdef __AVX__
    if (ilength > 16) {
      __m256 indexes = _mm256_set_ps(7.f, 6.f, 5.f, 4.f,
                                     3.f, 2.f, 1.f, 0.f);
      __m256 curindexes = indexes;
      __m256 values = _mm256_load_ps(input);
      const __m256 indexesStep = _mm256_set1_ps(8.f);
      for (int j = 8; j < ilength - 7; j += 8) {
        __m256 vec = _mm256_load_ps(input + j);
        __m256 cmpres;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
        if (min) {
          cmpres = _mm256_cmp_ps(vec, values, _CMP_GT_OQ);
        } else {
          cmpres = _mm256_cmp_ps(vec, values, _CMP_LT_OQ);
        }
#pragma GCC diagnostic pop
        curindexes = _mm256_add_ps(curindexes, indexesStep);
        values = _mm256_blendv_ps(vec, values, cmpres);
        indexes = _mm256_blendv_ps(curindexes, indexes, cmpres);
      }
      index = ElementAt(indexes, 0);
      extr = ElementAt(values, 0);
      for (int i = 1; i < 8; i++) {
        float val = ElementAt(values, i);
        if ((val < extr && min) || (val > extr && !min)) {
          index = ElementAt(indexes, i);
          extr = val;
        }
      }
      for (int j = ((ilength >> 3) << 3); j < ilength; j++) {
        float val = input[j];
        if ((val < extr && min) || (val > extr && !min)) {
          index = j;
          extr = val;
        }
      }
      return std::make_tuple(index, extr);
    }
  } else {
#elif defined(__ARM_NEON__)
    if (ilength > 8) {
      int32x4_t indexes = { 3, 2, 1, 0 };
      int32x4_t curindexes = indexes;
      float32x4_t values = vld1q_f32(input);
      const int32x4_t indexesStep = vdupq_n_s32(4);
      for (int j = 4; j < ilength - 3; j += 4) {
        float32x4_t vec = vld1q_f32(input + j);
        uint32x4_t cmpres;
        if (min) {
          cmpres = vcgtq_f32(vec, values);
        } else {
          cmpres = vcltq_f32(vec, values);
        }
        curindexes = vaddq_s32(curindexes, indexesStep);
        values = vbslq_f32(cmpres, vec, values);
        indexes = vbslq_s32(cmpres, curindexes, indexes);
      }
      index = vgetq_lane_s32(indexes, 0);
      extr = vgetq_lane_f32(values, 0);
      float val = vgetq_lane_f32(values, 1);
      if ((val < extr && min) || (val > extr && !min)) {
        index = vgetq_lane_s32(indexes, 1);
        extr = val;
      }
      val = vgetq_lane_f32(values, 2);
      if ((val < extr && min) || (val > extr && !min)) {
        index = vgetq_lane_s32(indexes, 2);
        extr = val;
      }
      val = vgetq_lane_f32(values, 3);
      if ((val < extr && min) || (val > extr && !min)) {
        index = vgetq_lane_s32(indexes, 3);
        extr = val;
      }

      for (int j = ((ilength >> 2) << 2); j < ilength; j++) {
        float val = input[j];
        if ((val < extr && min) || (val > extr && !min)) {
          index = j;
          extr = val;
        }
      }
      return std::make_tuple(index, extr);
    }
  } else {
#else
  } {
 #endif
    index = 0;
    extr = input[0];
    for (int j = 1; j < ilength; j++) {
      float val = input[j];
      if ((val < extr && min) || (val > extr && !min)) {
        index = j;
        extr = val;
      }
    }
  }
  return std::make_tuple(index, extr);
}

REGISTER_TRANSFORM(ArgMinMax);

}  // namespace transforms
}  // namespace sound_feature_extraction
