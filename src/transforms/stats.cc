/*! @file stats.cc
 *  @brief Statistical measures calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/stats.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <boost/regex.hpp>
#pragma GCC diagnostic pop
#include <cmath>
#include <simd/instruction_set.h>

namespace sound_feature_extraction {
namespace transforms {

std::set<StatsType> Parse(const std::string& value,
                          identity<std::set<StatsType>>) {
  static const std::unordered_map<std::string, StatsType> map {
    { internal::kStatsTypeAverageStr, kStatsTypeAverage },
    { internal::kStatsTypeStdDeviationStr, kStatsTypeStdDeviation },
    { internal::kStatsTypeSkewnessStr, kStatsTypeSkewness },
    { internal::kStatsTypeKurtosisStr, kStatsTypeKurtosis }
  };

  static const boost::regex all_regex("^\\s*(\\w+\\s*(\\s|$))+");
  boost::smatch match;
  if (!boost::regex_match(value, match, all_regex)) {
    throw InvalidParameterValueException();
  }

  if (value == internal::kStatsTypeAllStr) {
    return { kStatsTypeAverage, kStatsTypeStdDeviation, kStatsTypeSkewness,
             kStatsTypeKurtosis };
  }
  std::set<StatsType> ret;
  std::transform(boost::sregex_token_iterator(value.begin(), value.end(),
                                            boost::regex("\\s*(\\w+)\\s*"),
                                            1),
                 boost::sregex_token_iterator(),
                 std::inserter(ret, ret.begin()),
                 [](const std::string& subval) {
    auto stit = map.find(subval);
    if (stit == map.end()) {
      throw InvalidParameterValueException();
    }
    return stit->second;
  });

  return ret;
}

const std::unordered_map<int, Stats::CalculateFunc> Stats::kStatsFuncs {
  { kStatsTypeAverage, Stats::CalculateAverage },
  { kStatsTypeStdDeviation, Stats::CalculateStdDeviation },
  { kStatsTypeSkewness, Stats::CalculateSkewness },
  { kStatsTypeKurtosis, Stats::CalculateKurtosis }
};

Stats::Stats()
    : types_(kDefaultStatsTypes()),
      interval_(kDefaultInterval),
      overlap_(kDefaultOverlap) {
}

ALWAYS_VALID_TP(Stats, types)

bool Stats::validate_interval(const int& value) noexcept {
  return value >= 2;
}

bool Stats::validate_overlap(const int& value) noexcept {
  return value >= 0;
}

void Stats::Initialize() const {
  if (interval_ > 0 && overlap_ >= interval_) {
    throw InvalidParameterValueException("overlap", std::to_string(overlap_),
                                         HostName());
  }
}

size_t Stats::OnInputFormatChanged(size_t buffersCount) {
  if (interval_ != 0) {
    int step = interval_ - overlap_;
    auto ratio = (input_format_->Size() - interval_) / step + 1;
    if (((input_format_->Size() - interval_) % step) == 0) {
      output_format_->SetSize(ratio * types_.size());
    } else {
      output_format_->SetSize((ratio + 1) * types_.size());
    }
  } else {
    output_format_->SetSize(types_.size());
  }
  return buffersCount;
}

void Stats::Do(const float* in, float* out) const noexcept {
  float rawMoments[kStatsTypeCount];
  if (interval_ == 0) {
    CalculateRawMoments(true, in, 0, input_format_->Size(), rawMoments);
    Calculate(rawMoments, out);
  } else {
    size_t i;
    size_t step = interval_ - overlap_;
    for (i = 0; i < input_format_->Size() - interval_ + 1; i += step) {
      CalculateRawMoments(true, in, i, interval_, rawMoments);
      Calculate(rawMoments, out + i / step * types_.size());
    }
    if ((input_format_->Size() - interval_) % step != 0) {
      int index = input_format_->Size() - interval_;
      CalculateRawMoments(true, in, index, interval_, rawMoments);
      Calculate(rawMoments, out + i / step * types_.size());
    }
  }
}

void Stats::Calculate(const float* rawMoments, float* out) const noexcept {
  for (auto stat : types_) {
    int sind = 0;
    int istat = stat;
    while (istat >>= 1) {
      sind++;
    }
    out[sind] = kStatsFuncs.find(stat)->second(rawMoments);
  }
}

void Stats::CalculateRawMoments(bool simd, const float* in, int startIndex,
                                int length, float* rawMoments) noexcept {
  float avg1 = 0, avg2 = 0, avg3 = 0, avg4 = 0;
  auto end_index = startIndex + length;
  if (simd) {
#ifdef __AVX__
    __m256 avg1vec = _mm256_setzero_ps();
    __m256 avg2vec = _mm256_setzero_ps();
    __m256 avg3vec = _mm256_setzero_ps();
    __m256 avg4vec = _mm256_setzero_ps();
    for (int i = startIndex; i < end_index - 7; i+=8) {
      __m256 val = _mm256_loadu_ps(in + i);
      avg1vec = _mm256_add_ps(avg1vec, val);
      __m256 val2 = _mm256_mul_ps(val, val);
      avg2vec = _mm256_add_ps(avg2vec, val2);
      val = _mm256_mul_ps(val2, val);
      avg3vec = _mm256_add_ps(avg3vec, val);
      val2 = _mm256_mul_ps(val2, val2);
      avg4vec = _mm256_add_ps(avg4vec, val2);
    }
    avg1vec = _mm256_hadd_ps(avg1vec, avg1vec);
    avg1vec = _mm256_hadd_ps(avg1vec, avg1vec);
    avg1 += _mm256_get_ps(avg1vec, 0);
    avg1 += _mm256_get_ps(avg1vec, 4);
    avg2vec = _mm256_hadd_ps(avg2vec, avg2vec);
    avg2vec = _mm256_hadd_ps(avg2vec, avg2vec);
    avg2 += _mm256_get_ps(avg2vec, 0);
    avg2 += _mm256_get_ps(avg2vec, 4);
    avg3vec = _mm256_hadd_ps(avg3vec, avg3vec);
    avg3vec = _mm256_hadd_ps(avg3vec, avg3vec);
    avg3 += _mm256_get_ps(avg3vec, 0);
    avg3 += _mm256_get_ps(avg3vec, 4);
    avg4vec = _mm256_hadd_ps(avg4vec, avg4vec);
    avg4vec = _mm256_hadd_ps(avg4vec, avg4vec);
    avg4 += _mm256_get_ps(avg4vec, 0);
    avg4 += _mm256_get_ps(avg4vec, 4);
    for (int i = ((end_index) & ~0x7); i < end_index; i++) {
      float v = in[i];
      avg1 += v;
      float v2 = v * v;
      avg2 += v2;
      v *= v2;
      avg3 += v;
      v2 *= v2;
      avg4 += v2;
    }
  } else {
#elif defined(__ARM_NEON__)
    float32x4_t avg1vec = vdupq_n_f32(0);
    float32x4_t avg2vec = vdupq_n_f32(0);
    float32x4_t avg3vec = vdupq_n_f32(0);
    float32x4_t avg4vec = vdupq_n_f32(0);
    for (int i = startIndex; i < end_index - 3; i+=4) {
      float32x4_t val = vld1q_f32(in + i);
      avg1vec = vaddq_f32(avg1vec, val);
      float32x4_t val2 = vmulq_f32(val, val);
      avg2vec = vaddq_f32(avg2vec, val2);
      val = vmulq_f32(val2, val);
      avg3vec = vaddq_f32(avg3vec, val);
      val2 = vmulq_f32(val2, val2);
      avg4vec = vaddq_f32(avg4vec, val2);
    }
    avg1 += vgetq_lane_f32(avg1vec, 0);
    avg1 += vgetq_lane_f32(avg1vec, 1);
    avg1 += vgetq_lane_f32(avg1vec, 2);
    avg1 += vgetq_lane_f32(avg1vec, 3);
    avg2 += vgetq_lane_f32(avg2vec, 0);
    avg2 += vgetq_lane_f32(avg2vec, 1);
    avg2 += vgetq_lane_f32(avg2vec, 2);
    avg2 += vgetq_lane_f32(avg2vec, 3);
    avg3 += vgetq_lane_f32(avg3vec, 0);
    avg3 += vgetq_lane_f32(avg3vec, 1);
    avg3 += vgetq_lane_f32(avg3vec, 2);
    avg3 += vgetq_lane_f32(avg3vec, 3);
    avg4 += vgetq_lane_f32(avg4vec, 0);
    avg4 += vgetq_lane_f32(avg4vec, 1);
    avg4 += vgetq_lane_f32(avg4vec, 2);
    avg4 += vgetq_lane_f32(avg4vec, 3);
    for (int i = ((end_index) & ~0x3); i < end_index; i++) {
      float v = in[i];
      avg1 += v;
      float v2 = v * v;
      avg2 += v2;
      v *= v2;
      avg3 += v;
      v2 *= v2;
      avg4 += v2;
    }
  } else {
#else
  } {
#endif
    for (int i = startIndex; i < end_index; i++) {
      float v = in[i];
      avg1 += v;
      float v2 = v * v;
      avg2 += v2;
      v *= v2;
      avg3 += v;
      v2 *= v2;
      avg4 += v2;
    }
  }
  avg1 /= length;
  avg2 /= length;
  avg3 /= length;
  avg4 /= length;
  rawMoments[0] = avg1;
  rawMoments[1] = avg2;
  rawMoments[2] = avg3;
  rawMoments[3] = avg4;
}

float Stats::CalculateAverage(const float* rawMoments) noexcept {
  return rawMoments[0];
}

float Stats::CalculateStdDeviation(const float* rawMoments) noexcept {
  auto value = rawMoments[1] - rawMoments[0] * rawMoments[0];
  if (value < 0) {
    return 0;
  }
  value = sqrtf(value);
  return value;
}

float Stats::CalculateSkewness(const float* rawMoments) noexcept {
  float avg1 = rawMoments[0];
  float avg2 = rawMoments[1];
  float avg3 = rawMoments[2];
  double u2 = avg2 - avg1 * avg1;
  if (u2 <= 0) {
    return 0;
  }
  double u3 = avg3 - 3 * avg2 * avg1 + 2 * avg1 * avg1 * avg1;
  auto value = u3 / (sqrt(u2) * u2);
  return value;
}

float Stats::CalculateKurtosis(const float* rawMoments) noexcept {
  float avg1 = rawMoments[0];
  float avg2 = rawMoments[1];
  float avg3 = rawMoments[2];
  float avg4 = rawMoments[3];
  double u2 = avg2 - avg1 * avg1;
  if (u2 == 0) {
    return -2.f;
  }
  double u4 = avg4 - 4 * avg3 * avg1 + 6 * avg2 * avg1 * avg1
      - 3 * avg1 * avg1 * avg1 * avg1;
  auto value = u4 / (u2 * u2) - 3;
  return value;
}

RTP(Stats, types)
RTP(Stats, interval)
RTP(Stats, overlap)
REGISTER_TRANSFORM(Stats);

}  // namespace transforms
}  // namespace sound_feature_extraction
