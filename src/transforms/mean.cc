/*! @file mean.cc
 *  @brief Arithmetic and geometric means calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/mean.h"
#include <algorithm>
#include <limits>
#ifdef __AVX__
#include <simd/avx_mathfun.h>
#elif defined(__ARM_NEON__)
#include <simd/neon_mathfun.h>
#endif
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <boost/regex.hpp>
#pragma GCC diagnostic pop
#include <simd/arithmetic-inl.h>

namespace sound_feature_extraction {
namespace transforms {

using formats::FixedArray;

std::set<MeanType> Parse(const std::string& value,
                         identity<std::set<MeanType>>) {
  static const std::unordered_map<std::string, MeanType> map {
    { internal::kMeanTypeArithmeticStr, kMeanTypeArithmetic },
    { internal::kMeanTypeGeometricStr, kMeanTypeGeometric },
  };

  static const boost::regex all_regex("^\\s*(\\w+\\s*(\\s|$))+");
  boost::smatch match;
  if (!boost::regex_match(value, match, all_regex)) {
    throw InvalidParameterValueException();
  }

  std::set<MeanType> ret;
  std::transform(boost::sregex_token_iterator(value.begin(), value.end(),
                                            boost::regex("\\s*(\\w+)\\s*"),
                                            1),
                 boost::sregex_token_iterator(),
                 std::inserter(ret, ret.begin()),
                 [](const std::string& subval) {
    auto mtypeit = map.find(subval);
    if (mtypeit == map.end()) {
      throw InvalidParameterValueException();
    }
    return mtypeit->second;
  });

  return ret;
}

Mean::Mean() : types_(kDefaultMeanTypes()) {
}

ALWAYS_VALID_TP(Mean, types)

void Mean::Do(const float* in,
            FixedArray<kMeanTypeCount>* out) const noexcept {
  for (int j = 0; j < kMeanTypeCount; j++) {
    auto mt = static_cast<MeanType>(j);
    if (types_.find(mt) != types_.end()) {
      (*out)[j] = Do(use_simd(), in, input_format_->Size(), mt);
    } else {
      (*out)[j] = 0;
    }
  }
}

float Mean::Do(bool simd, const float* input, size_t length,
               MeanType type) noexcept {
  int ilength = static_cast<int>(length);
  switch (type) {
    case kMeanTypeArithmetic: {
      float res;
      if (simd) {
#if defined(__AVX__) || defined(__ARM_NEON__)
        res = sum_elements(input, length);
      } else {
#else
      } {
#endif
        res = sum_elements_na(input, length);
      }
      res /= length;
      return res;
    }
    case kMeanTypeGeometric: {
      const float power = 1.f / ilength;
      if (simd) {
#ifdef __AVX__
        __m256 res = _mm256_set1_ps(1.f), tmp = _mm256_set1_ps(1.f);
        const __m256 powvec = _mm256_set1_ps(power);
        const __m256 infvec = _mm256_set1_ps(
            std::numeric_limits<float>::infinity());
        for (int j = 0; j < ilength - 7; j += 8) {
          __m256 vec = _mm256_load_ps(input + j);
          __m256 mulvec = _mm256_mul_ps(tmp, vec);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
          __m256 cmpvec = _mm256_cmp_ps(mulvec, infvec, _CMP_EQ_UQ);
          int check = _mm256_movemask_ps(cmpvec);
          if (check != 0) {
            // Taking a power from 0 can lead to unexpected results...
            // Apply the mask to workaround zeros in tmp
            cmpvec = _mm256_cmp_ps(tmp, _mm256_set1_ps(0.f), _CMP_EQ_UQ);
            tmp = pow256_ps(tmp, powvec);
            tmp = _mm256_blendv_ps(tmp, _mm256_set1_ps(0.f), cmpvec);
            res = _mm256_mul_ps(res, tmp);
            tmp = vec;
          } else {
            tmp = mulvec;
          }
#pragma GCC diagnostic pop
        }
        for (int i = 0; i < 8; i++) {
          if (ElementAt(tmp, i) == 0) {
            return 0;
          }
        }
        tmp = pow256_ps(tmp, powvec);
        res = _mm256_mul_ps(res, tmp);
        float sctmp = 1.f;
        for (int j = (ilength & ~0x7); j < ilength; j++) {
          sctmp *= input[j];
        }
        float scres = powf(sctmp, power);
        for (int j = 0; j < 8; j++) {
          scres *= ElementAt(res, j);
        }
        return scres;
      } else {
#elif defined(__ARM_NEON__)
        float32x4_t res = vdupq_n_f32(1.0f), tmp = vdupq_n_f32(1.0f),
            powvec = vdupq_n_f32(power),
            infvec = vdupq_n_f32(std::numeric_limits<float>::infinity());

        for (int j = 0; j < ilength - 7; j += 8) {
          float32x4_t vec = vld1q_f32(input + j);
          float32x4_t mulvec = vmulq_f32(tmp, vec);
          uint32x4_t cmpvec = vceqq_f32(mulvec, infvec);
          uint64x2_t cmpvec2 = vpaddlq_u32(cmpvec);
          if (vgetq_lane_u64(cmpvec2, 0) != 0 || vgetq_lane_u64(cmpvec2, 1) != 0) {
            tmp = pow_ps(tmp, powvec);
            res = vmulq_f32(res, tmp);
            tmp = vec;
          } else {
            tmp = mulvec;
          }
        }
        tmp = pow_ps(tmp, powvec);
        res = vmulq_f32(res, tmp);
        float sctmp = 1.f;
        for (int j = (ilength & ~0x3); j < ilength; j += 2) {
          sctmp *= input[j];
        }
        float scres = powf(sctmp, power);
        scres *= vgetq_lane_f32(res, 0) * vgetq_lane_f32(res, 1) *
            vgetq_lane_f32(res, 2) * vgetq_lane_f32(res, 3);
        return scres;
      } else {
#else
      } {
#endif
        float res = 1.f, tmp = 1.f;
        for (int j = 0; j < ilength; j++) {
          float val = input[j];
          float multmp = tmp * val;
          if (multmp == std::numeric_limits<float>::infinity()) {
            res *= powf(tmp, power);
            tmp = val;
          } else {
            tmp = multmp;
          }
        }
        if (tmp != 1.f) {
          res *= powf(tmp, power);
        }
        return res;
      }
    }
    default:
      break;
  }
  return 0.f;
}

RTP(Mean, types)
REGISTER_TRANSFORM(Mean);

}  // namespace transforms
}  // namespace sound_feature_extraction
