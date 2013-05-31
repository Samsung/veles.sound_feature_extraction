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

Stats::Stats() : types_({ STATS_TYPE_AVERAGE, STATS_TYPE_DISPERSION,
                          STATS_TYPE_SKEW, STATS_TYPE_KURTOSIS}) {
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
}

void Stats::InitializeBuffers(
    const BuffersBase<float>& in UNUSED,
    BuffersBase<StatsArray>* buffers) const noexcept {
  buffers->Initialize(1);
}

void Stats::Do(const BuffersBase<float>& in,
               BuffersBase<StatsArray>* out) const noexcept {
  for (auto stat : types_) {
    int ind = 0;
    int istat = stat;
    while (istat >>= 1) {
      ind++;
    }
    (*out)[0][ind] = kStatsFuncs.find(stat)->second(in);
  }
}

float Stats::CalculateAverage(const BuffersBase<float>& in) noexcept {
  double avg1 = 0;
  for (size_t i = 0; i < in.Size(); i++) {
    avg1 += in[i];
  }
  return avg1 / in.Size();
}

float Stats::CalculateDispersion(const BuffersBase<float>& in) noexcept {
  double avg1 = 0, avg2 = 0;
  for (size_t i = 0; i < in.Size(); i++) {
    double v = in[i];
    avg1 += v;
    avg2 += v * v;
  }
  avg1 /= in.Size();
  avg2 /= in.Size();
  return avg2 - avg1 * avg1;
}

float Stats::CalculateSkew(const BuffersBase<float>& in) noexcept {
  double avg1 = 0, avg2 = 0, avg3 = 0;
  for (size_t i = 0; i < in.Size(); i++) {
    double v = in[i];
    avg1 += v;
    avg2 += v * v;
    avg3 += v * v * v;
  }
  avg1 /= in.Size();
  avg2 /= in.Size();
  avg3 /= in.Size();
  double u2 = avg2 - avg1 * avg1;
  double u3 = avg3 - 3 * avg2 * avg1 + 2 * avg1 * avg1 * avg1;
  return u3 / (sqrt(u2) * u2);
}

float Stats::CalculateKurtosis(const BuffersBase<float>& in) noexcept {
  double avg1 = 0, avg2 = 0, avg3 = 0, avg4 = 0;
  for (size_t i = 0; i < in.Size(); i++) {
    double v = in[i];
    avg1 += v;
    avg2 += v * v;
    avg3 += v * v * v;
    avg4 += v * v * v * v;
  }
  avg1 /= in.Size();
  avg2 /= in.Size();
  avg3 /= in.Size();
  avg4 /= in.Size();
  double u2 = avg2 - avg1 * avg1;
  double u4 = avg4 - 4 * avg3 * avg1 + 6 * avg2 * avg1 * avg1
      - 3 * avg1 * avg1 * avg1 * avg1;
  return u4 / (u2 * u2) - 3;
}

REGISTER_TRANSFORM(Stats);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
