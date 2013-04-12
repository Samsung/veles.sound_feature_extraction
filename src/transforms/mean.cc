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
#include <math.h>
#ifdef __AVX__
#include <immintrin.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>
#endif
#include <boost/regex.hpp>
#include "src/primitives/energy.h"
#include "src/primitives/avx_extra.h"

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
#ifdef __AVX__
        __m256 accum = _mm256_setzero_ps();
        for (int j = 0; j < ilength - 7; j += 8) {
          __m256 vec = _mm256_load_ps(input + j);
          accum = _mm256_add_ps(accum, vec);
        }
        accum = _mm256_hadd_ps(accum, accum);
        accum = _mm256_hadd_ps(accum, accum);
        res = ElementAt(accum, 0) + ElementAt(accum, 4);
        for (int j = ((ilength >> 3) << 3); j < ilength; j++) {
          res += input[j];
        }
      } else {
#elif defined(__ARM_NEON__)
        float32x4_t accum = { .0f, .0f, .0f, .0f };
        for (int j = 0; j < ilength - 3; j += 4) {
          float32x4_t vec = vld1q_f32(input + j);
          accum = vaddq_f32(accum, vec);
        }
        res = accum[0] + accum[1] + accum[2] + accum[3];
        for (int j = ((ilength >> 2) << 2); j < ilength; j += 2) {
          res += input[j];
        }
      } else {
#else
      } {
 #endif
        res = .0f;
        for (int j = 0; j < ilength; j++) {
          res += input[j];
        }
      }
      res /= length;
      return res;
    }
    case MEAN_TYPE_GEOMETRIC: {
      float res = 1.f, tmp = 1.f, power = 1.f / ilength;
      for (int j = 0; j < ilength; j++) {
        tmp *= input[j];
        if (j % 8 == 0) {
          res *= powf(tmp, power);
          tmp = 1.f;
        }
      }
      if (tmp != 1.f) {
        res *= powf(tmp, power);
      }
      return res;
    }
    default:
      break;
  }
  return .0f;
}

REGISTER_TRANSFORM(Mean);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
