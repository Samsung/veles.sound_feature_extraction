/*! @file stats.h
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

#ifndef SRC_TRANSFORMS_STATS_H_
#define SRC_TRANSFORMS_STATS_H_

#include <set>
#include "src/formats/array_format.h"
#include "src/omp_transform_base.h"

namespace sound_feature_extraction {
namespace transforms {

enum StatsType {
  kStatsTypeAverage = 1,
  kStatsTypeStdDeviation = 2,
  kStatsTypeSkewness = 4,
  kStatsTypeKurtosis = 8,
  kStatsTypeCount = 4
};

namespace internal {
constexpr const char* kStatsTypeAllStr = "all";
constexpr const char* kStatsTypeAverageStr = "average";
constexpr const char* kStatsTypeStdDeviationStr = "stddev";
constexpr const char* kStatsTypeSkewnessStr = "skewness";
constexpr const char* kStatsTypeKurtosisStr = "kurtosis";
}

std::set<StatsType> Parse(const std::string& value,
                          identity<std::set<StatsType>>);

}  // namespace transforms
}  // namespace sound_feature_extraction

namespace std {
  inline string to_string(
      const set<sound_feature_extraction::transforms::StatsType>& value)
      noexcept {
    string res;
    for (auto type : value) {
      switch (type) {
        case sound_feature_extraction::transforms::kStatsTypeAverage:
          res += sound_feature_extraction::transforms::
              internal::kStatsTypeAverageStr;
          res += " ";
          break;
        case sound_feature_extraction::transforms::kStatsTypeStdDeviation:
          res += sound_feature_extraction::transforms::
              internal::kStatsTypeStdDeviationStr;
          res += " ";
          break;
        case sound_feature_extraction::transforms::kStatsTypeSkewness:
          res += sound_feature_extraction::transforms::
              internal::kStatsTypeSkewnessStr;
          res += " ";
          break;
        case sound_feature_extraction::transforms::kStatsTypeKurtosis:
          res += sound_feature_extraction::transforms::
              internal::kStatsTypeKurtosisStr;
          res += " ";
          break;
        default:
          break;
      }
    }
    return res.empty()? "" : res.substr(0, res.size() - 1);
  }
}  // namespace std

namespace sound_feature_extraction {
namespace transforms {

class Stats : public OmpTransformBase<formats::ArrayFormatF,
                                      formats::ArrayFormatF>  {
 public:
  Stats();

  TRANSFORM_INTRO("Stats", "Calculate statistical measures, such as skew "
                           "or kurtosis.",
                  Stats)

  TP(types, std::set<StatsType>, kDefaultStatsTypes(),
     "Stats types to calculate (names separated with spaces, \"all\" for all).")
  TP(interval, int, kDefaultInterval,
     "\"Texture\" interval, must be bigger than 1. Zero means all buffers.")
  TP(overlap, int, kDefaultOverlap,
     "Overlap value, cannot be greater than (interval - 1).")

  virtual void Initialize() const override;

 protected:
  typedef float(*CalculateFunc)(const float*);

  static std::set<StatsType> kDefaultStatsTypes() noexcept {
    return { kStatsTypeAverage, kStatsTypeStdDeviation,
             kStatsTypeSkewness, kStatsTypeKurtosis };
  }
  static constexpr int kDefaultInterval = 0;
  static constexpr int kDefaultOverlap = 0;
  static const std::unordered_map<int, CalculateFunc> kStatsFuncs;

  virtual size_t OnInputFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in, float* out) const noexcept override;

  void Calculate(const float* rawMoments, float* out) const noexcept;
  static void CalculateRawMoments(bool simd, const float* in, int startIndex,
                                  int length, float* rawMoments) noexcept;
  static float CalculateAverage(const float* rawMoments) noexcept;
  static float CalculateStdDeviation(const float* rawMoments) noexcept;
  static float CalculateSkewness(const float* rawMoments) noexcept;
  static float CalculateKurtosis(const float* rawMoments) noexcept;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_STATS_H_
