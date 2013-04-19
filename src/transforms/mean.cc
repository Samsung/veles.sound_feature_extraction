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
#ifdef __AVX__
#include "src/primitives/avx_mathfun.h"
#elif defined(__ARM_NEON__)
#include "src/primitives/neon_mathfun.h"
#endif
#include <boost/regex.hpp>
#include <limits>
#include "src/primitives/arithmetic-inl.h"

namespace SoundFeatureExtraction {
namespace Transforms {

using Formats::FixedArray;

const std::unordered_map<std::string, MeanTypes> Mean::MeanTypesMap {
  { "arithmetic", MEAN_TYPE_ARITHMETIC },
  { "geometric", MEAN_TYPE_GEOMETRIC },
};
const std::set<MeanTypes> Mean::kDefaultMeanTypes { MEAN_TYPE_ARITHMETIC };
const std::string Mean::kDefaultMeanTypesStr = "arithmetic";

Mean::Mean() : types_(kDefaultMeanTypes) {
  RegisterSetter("types", [&](const std::string& value) {
    static const boost::regex allRegex("^\\s*(\\w+\\s*(\\s|$))+");
    boost::smatch match;
    if (!boost::regex_match(value, match, allRegex)) {
      return false;
    }
    static const boost::regex typesRegex("\\s*(\\w+)\\s*");
    static const boost::sregex_token_iterator empty;
    boost::sregex_token_iterator typesIterator(
          value.begin(), value.end(), typesRegex, 1);
    assert(typesIterator != empty);
    while (typesIterator != empty) {
      auto mtypeit = MeanTypesMap.find(*typesIterator++);
      if (mtypeit == MeanTypesMap.end()) {
        return false;
      }
      types_.insert(mtypeit->second);
    }
    return true;
  });
}

void Mean::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<FixedArray<MEAN_TYPE_COUNT>>* buffers) const noexcept {
  buffers->Initialize(in.Size());
}

void Mean::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<FixedArray<MEAN_TYPE_COUNT>> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    for (int j = 0; j < MEAN_TYPE_COUNT; j++) {
      auto mt = static_cast<MeanTypes>(j);
      if (types_.find(mt) != types_.end()) {
        (*(*out)[i])[j] = Do(true, in[i]->Data.get(), inputFormat_->Size(), mt);
      } else {
        (*(*out)[i])[j] = 0;
      }
    }
  }
}

float Mean::Do(bool simd, const float* input, size_t length,
               MeanTypes type) noexcept {
  int ilength = static_cast<int>(length);
  switch (type) {
    case MEAN_TYPE_ARITHMETIC: {
      float res;
      if (simd) {
#if defined(__AVX__) || defined(__ARM_NEON__)
        res = sum(input, length);
      } else {
#else
      } {
#endif
        res = sum_na(input, length);
      }
      res /= length;
      return res;
    }
    case MEAN_TYPE_GEOMETRIC: {
      const float power = 1.f / ilength;
      if (simd) {
#ifdef __AVX__
        __m256 res = _mm256_set1_ps(1.f), tmp = _mm256_set1_ps(1.f);
        const __m256 powvec = _mm256_set1_ps(power);
        const __m256 infvec = _mm256_set1_ps(std::numeric_limits<float>::infinity());
        for (int j = 0; j < ilength - 7; j += 8) {
          __m256 vec = _mm256_load_ps(input + j);
          __m256 mulvec = _mm256_mul_ps(tmp, vec);
          __m256 cmpvec = _mm256_cmp_ps(mulvec, infvec, _CMP_EQ_UQ);
          // I do not know how to check fast if at least one element is inf;
          // Using 2 hadd and 2 comparisons
          cmpvec = _mm256_hadd_ps(cmpvec, cmpvec);
          cmpvec = _mm256_hadd_ps(cmpvec, cmpvec);
          if (ElementAt(cmpvec, 0) != 0 || ElementAt(cmpvec, 1) != 0) {
            tmp = pow256_ps(tmp, powvec);
            res = _mm256_mul_ps(res, tmp);
            tmp = vec;
          } else {
            tmp = mulvec;
          }
        }
        tmp = pow256_ps(tmp, powvec);
        res = _mm256_mul_ps(res, tmp);
        float sctmp = 1.f;
        for (int j = ((ilength >> 3) << 3); j < ilength; j++) {
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
        for (int j = ((ilength >> 2) << 2); j < ilength; j += 2) {
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

REGISTER_TRANSFORM(Mean);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
