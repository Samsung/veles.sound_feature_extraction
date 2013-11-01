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
#include "src/formats/fixed_array.h"
#include "src/formats/array_format.h"
#include "src/formats/single_format.h"
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

class Stats
    : public OmpTransformBase<formats::ArrayFormatF,
                              formats::ArrayFormat<
                                  formats::FixedArray<kStatsTypeCount>>> {
 public:
  typedef formats::FixedArray<kStatsTypeCount> StatsArray;

  Stats();

  TRANSFORM_INTRO("Stats", "Calculate statistical measures, such as skew "
                           "or kurtosis.")

  OMP_TRANSFORM_PARAMETERS(
      TP("types", "Stats types to calculate (names separated with spaces, "
                  "\"all\" for all).",
         "all")
      TP("interval", "\"Texture\" interval, must be bigger than 1. "
                     "Zero means all buffers.",
         "0")
  )

 protected:
  typedef float(*CalculateFunc)(const float*);

  static const std::unordered_map<std::string, StatsType> kStatsTypesMap;
  static const std::unordered_map<int, CalculateFunc> kStatsFuncs;

  virtual size_t OnInputFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in, StatsArray* out) const noexcept override;

  void Calculate(const float* rawMoments, StatsArray* out) const noexcept;
  static void CalculateRawMoments(bool simd, const float* in, int startIndex,
                                  int length, float* rawMoments) noexcept;
  static float CalculateAverage(const float* rawMoments) noexcept;
  static float CalculateStdDeviation(const float* rawMoments) noexcept;
  static float CalculateSkew(const float* rawMoments) noexcept;
  static float CalculateKurtosis(const float* rawMoments) noexcept;

private:
  std::set<StatsType> types_;
  int interval_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_STATS_H_
