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
    : public TransformBase<Formats::SingleFormatF,
                           Formats::SingleFormat<
                               Formats::FixedArray<STATS_TYPE_COUNT>>> {
 public:
  typedef Formats::FixedArray<STATS_TYPE_COUNT> StatsArray;

  Stats();

  TRANSFORM_INTRO("Stats", "Calculate statistical measures, such as skew "
                           "or kurtosis.")

  TRANSFORM_PARAMETERS(
      TP("types", "Stats types to calculate (names separated with spaces, "
                  "\"all\" for all).",
         "all")
  )

 protected:
  typedef float(*CalculateFunc)(const BuffersBase<float>&);

  static const std::unordered_map<std::string, StatsType> kStatsTypesMap;
  static const std::unordered_map<int, CalculateFunc> kStatsFuncs;

  virtual void InitializeBuffers(
      const BuffersBase<float>& in,
      BuffersBase<StatsArray>* buffers) const noexcept;

  virtual void Do(const BuffersBase<float>& in,
                  BuffersBase<StatsArray>* out)
  const noexcept;

  static float CalculateAverage(const BuffersBase<float>& in) noexcept;
  static float CalculateDispersion(const BuffersBase<float>& in) noexcept;
  static float CalculateSkew(const BuffersBase<float>& in) noexcept;
  static float CalculateKurtosis(const BuffersBase<float>& in) noexcept;

private:
  std::set<StatsType> types_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_STATS_H_
