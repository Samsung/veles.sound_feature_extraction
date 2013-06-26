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
#include <boost/regex.hpp>
#include <math.h>

namespace SoundFeatureExtraction {
namespace Transforms {

const std::unordered_map<std::string, StatsType> Stats::kStatsTypesMap {
  { "average", STATS_TYPE_AVERAGE },
  { "dispersion", STATS_TYPE_DISPERSION },
  { "skew", STATS_TYPE_SKEW},
  { "kurtosis", STATS_TYPE_KURTOSIS }
};

const std::unordered_map<int, Stats::CalculateFunc> Stats::kStatsFuncs {
  { STATS_TYPE_AVERAGE, Stats::CalculateAverage },
  { STATS_TYPE_DISPERSION, Stats::CalculateDispersion },
  { STATS_TYPE_SKEW, Stats::CalculateSkew },
  { STATS_TYPE_KURTOSIS, Stats::CalculateKurtosis }
};

Stats::Stats()
    : types_({ STATS_TYPE_AVERAGE, STATS_TYPE_DISPERSION,
               STATS_TYPE_SKEW, STATS_TYPE_KURTOSIS}),
      interval_(0) {
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
      auto type_name = *typesIterator++;
      if (type_name == "all") {
        for (auto tmp : kStatsTypesMap) {
          types_.insert(tmp.second);
        }
      } else {
        auto mtypeit = kStatsTypesMap.find(type_name);
        if (mtypeit == kStatsTypesMap.end()) {
          return false;
        }
        types_.insert(mtypeit->second);
      }
    }
    return true;
  });
  RegisterSetter("interval", [&](const std::string& value) {
    int iv = Parse<int>("interval", value);
    if (iv < 2) {
      return false;
    }
    interval_ = iv;
    return true;
  });
}

BuffersCountChange Stats::OnInputFormatChanged() {
  if (interval_ != 0) {
    auto interval = interval_;
    return BuffersCountChange([=](size_t inSize) {
      if (inSize % interval == 0) {
        return inSize / interval;
      }
      return inSize / interval + 1;
    });
  } else {
    return BuffersCountChange(1);
  }
}

void Stats::Do(const BuffersBase<float>& in,
               BuffersBase<StatsArray>* out) const noexcept {
  float rawMoments[4];
  if (interval_ == 0) {
    CalculateRawMoments(in, 0, in.Count(), rawMoments);
    Calculate(rawMoments, 0, out);
  } else {
    size_t i;
    for (i = 0; i < in.Count() - interval_ + 1; i+= interval_) {
      CalculateRawMoments(in, i, interval_, rawMoments);
      Calculate(rawMoments, i / interval_, out);
    }
    if (i % interval_ != 0) {
      CalculateRawMoments(in, i, in.Count() - i, rawMoments);
      Calculate(rawMoments, i / interval_ + 1, out);
    }
  }
}

void Stats::Calculate(const float* rawMoments, int index,
                      BuffersBase<StatsArray>* out) const noexcept {
  for (auto stat : types_) {
    int sind = 0;
    int istat = stat;
    while (istat >>= 1) {
      sind++;
    }
    (*out)[index][sind] = kStatsFuncs.find(stat)->second(rawMoments);
  }
}

void Stats::CalculateRawMoments(const BuffersBase<float>& in,
                                int startIndex, int length,
                                float* rawMoments) noexcept {
  double avg1 = 0, avg2 = 0, avg3 = 0, avg4 = 0;
  for (int i = startIndex; i < startIndex + length; i++) {
    double v = in[i];
    avg1 += v;
    avg2 += v * v;
    avg3 += v * v * v;
    avg4 += v * v * v * v;
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

float Stats::CalculateDispersion(const float* rawMoments) noexcept {
  return rawMoments[1] - rawMoments[0] * rawMoments[0];
}

float Stats::CalculateSkew(const float* rawMoments) noexcept {
  float avg1 = rawMoments[0];
  float avg2 = rawMoments[1];
  float avg3 = rawMoments[2];
  double u2 = avg2 - avg1 * avg1;
  if (u2 == 0) {
    return 0;
  }
  double u3 = avg3 - 3 * avg2 * avg1 + 2 * avg1 * avg1 * avg1;
  return u3 / (sqrt(u2) * u2);
}

float Stats::CalculateKurtosis(const float* rawMoments) noexcept {
  float avg1 = rawMoments[0];
  float avg2 = rawMoments[1];
  float avg3 = rawMoments[2];
  float avg4 = rawMoments[3];
  double u2 = avg2 - avg1 * avg1;
  if (u2 == 0) {
    return 1e10f;
  }
  double u4 = avg4 - 4 * avg3 * avg1 + 6 * avg2 * avg1 * avg1
      - 3 * avg1 * avg1 * avg1 * avg1;
  return u4 / (u2 * u2) - 3;
}

REGISTER_TRANSFORM(Stats);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
