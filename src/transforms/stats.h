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
#include "src/formats/raw_format.h"
#include "src/formats/single_format.h"
#include "src/omp_transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

enum StatsType {
  STATS_TYPE_AVERAGE = 1,
  STATS_TYPE_DISPERSION = 2,
  STATS_TYPE_SKEW = 4,
  STATS_TYPE_KURTOSIS = 8,
  STATS_TYPE_COUNT = 4
};

class Stats
    : public OmpTransformBase<Formats::RawFormatF,
                              Formats::RawFormat<
                                  Formats::FixedArray<STATS_TYPE_COUNT>>> {
 public:
  typedef Formats::FixedArray<STATS_TYPE_COUNT> StatsArray;

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

  void Calculate(const float* rawMoments, int index,
                 StatsArray* out) const noexcept;
  static void CalculateRawMoments(bool simd, const float* in, int startIndex,
                                  int length, float* rawMoments) noexcept;
  static float CalculateAverage(const float* rawMoments) noexcept;
  static float CalculateDispersion(const float* rawMoments) noexcept;
  static float CalculateSkew(const float* rawMoments) noexcept;
  static float CalculateKurtosis(const float* rawMoments) noexcept;

private:
  std::set<StatsType> types_;
  int interval_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_STATS_H_
