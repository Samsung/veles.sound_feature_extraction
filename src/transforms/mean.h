/*! @file mean.h
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

#ifndef SRC_TRANSFORMS_MEAN_H_
#define SRC_TRANSFORMS_MEAN_H_

#include <set>
#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

enum MeanTypes {
  MEAN_TYPE_ARITHMETIC = 0,
  MEAN_TYPE_GEOMETRIC,
  MEAN_TYPE_COUNT
};

class Mean
    : public OmpTransformBase<formats::ArrayFormatF,
                              formats::SingleFormat<
                                  formats::FixedArray<MEAN_TYPE_COUNT>>> {
 public:
  Mean();

  TRANSFORM_INTRO("Mean", "Window means calculation.")

  OMP_TRANSFORM_PARAMETERS(
      TP("types", "Mean types to calculate (names separated with spaces).",
         kDefaultMeanTypesStr)
  )

 protected:
  static const std::unordered_map<std::string, MeanTypes> kMeanTypesMap;

  virtual void Do(const float* in,
                  formats::FixedArray<MEAN_TYPE_COUNT>* out)
      const noexcept override;

  static float Do(bool simd, const float* input, size_t length,
                  MeanTypes type) noexcept;

private:
  static const std::set<MeanTypes> kDefaultMeanTypes;
  static const std::string kDefaultMeanTypesStr;

  std::set<MeanTypes> types_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_MEAN_H_
