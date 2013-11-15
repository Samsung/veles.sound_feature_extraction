/*! @file delta.h
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_DELTA_H_
#define SRC_TRANSFORMS_DELTA_H_

#include "src/formats/array_format.h"
#include "src/transform_base.h"

namespace sound_feature_extraction {
namespace transforms {

enum DeltaType {
  kDeltaTypeSimple,
  kDeltaTypeRegression
};

namespace internal {
constexpr const char* kDeltaTypeSimpleStr = "simple";
constexpr const char* kDeltaTypeRegressionStr = "regression";
}

DeltaType Parse(const std::string& value, identity<DeltaType>);

class Delta : public UniformFormatTransform<formats::ArrayFormatF> {
 public:
  Delta();

  TRANSFORM_INTRO("Delta", "Get the difference between values of adjacent "
                           "windows.",
                  Delta)

      TP(type, DeltaType, kDefaultDeltaType,
         "The algorithm to calculate the deltas with. Allowed values "
         "are \"simple\" and \"regression\".")
      TP(rlength, int, kDefaultRegressionLength,
         "The linear regression window length. Only odd values "
         " greater than 1 are accepted.")

 protected:
  static constexpr DeltaType kDefaultDeltaType = kDeltaTypeSimple;
  static constexpr int kDefaultRegressionLength = 5;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*>* out) const noexcept override;

  static void DoSimple(bool simd, const float* prev, const float* cur,
                       size_t length, float* res) noexcept;

  static void DoRegression(bool simd, const BuffersBase<float*>& in,
                           int rstep, int i, float norm, int windowSize,
                           BuffersBase<float*>* out) noexcept;
};

}  // namespace transforms
}  // namespace sound_feature_extraction

namespace std {
  inline string
  to_string(sound_feature_extraction::transforms::DeltaType value) noexcept {
    switch (value) {
      case sound_feature_extraction::transforms::kDeltaTypeSimple:
        return sound_feature_extraction::transforms::internal::kDeltaTypeSimpleStr;
      case sound_feature_extraction::transforms::kDeltaTypeRegression:
        return sound_feature_extraction::transforms::internal::kDeltaTypeRegressionStr;
    }
    return "";
  }
}  // namespace std

#endif  // SRC_TRANSFORMS_DELTA_H_
