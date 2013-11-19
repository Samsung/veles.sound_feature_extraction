/*! @file filter_bank.h
 *  @brief Psychoacoustic scale conversion.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_FILTER_BANK_H_
#define SRC_TRANSFORMS_FILTER_BANK_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

enum class ScaleType {
  kLinear,
  kMel,
  kBark
};

namespace internal {
constexpr const char* kScaleTypeLinearStr = "linear";
constexpr const char* kScaleTypeMelStr = "mel";
constexpr const char* kScaleTypeBarkStr = "bark";
}

ScaleType Parse(const std::string& value, identity<ScaleType>);

}  // namespace transforms
}  // namespace sound_feature_extraction

namespace std {
  using sound_feature_extraction::transforms::ScaleType;

  inline string
  to_string(const ScaleType& scale) noexcept {
    switch (scale) {
      case ScaleType::kLinear:
        return sound_feature_extraction::transforms::internal::
            kScaleTypeLinearStr;
      case ScaleType::kMel:
        return sound_feature_extraction::transforms::internal::
            kScaleTypeMelStr;
      case ScaleType::kBark:
        return sound_feature_extraction::transforms::internal::
            kScaleTypeBarkStr;
    }
    return "";
  }
}  // namespace std

namespace sound_feature_extraction {
namespace transforms {

class FilterBank : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  FilterBank();

  TRANSFORM_INTRO("FilterBank",
                  "Converts the signal to the selected psychoacoustic scale "
                  "(default is mel).",
                  FilterBank)

  TP(type, ScaleType, kDefaultScale,
     "The type of the scale. Supported values are \"linear\","
     "\"mel\" and \"bark\".")
  TP(number, int, kDefaultNumber,
     "The number of triangular filters.")
  TP(frequency_min, int, kDefaultMinFrequency,
     "Minimal frequency of the filter bank.")
  TP(frequency_max, int, kDefaultMaxFrequency,
     "Maximal frequency of the filter bank.")

  virtual void Initialize() const override;

 protected:
  static constexpr ScaleType kDefaultScale = ScaleType::kMel;
  static constexpr int kDefaultNumber = 40;
  static constexpr int kDefaultMinFrequency = 130;
  static constexpr int kDefaultMaxFrequency = 6854;

  virtual void Do(const float* in,
                  float* out) const noexcept override;

  static float LinearToScale(ScaleType type, float freq);
  static float ScaleToLinear(ScaleType type, float value);

  const FloatPtr& filter_bank() const;

 private:
  /// @brief Adds a triangular filter to the filter bank.
  /// @param center The value of the peak of the triangle,
  /// in psychoacoustic scale units.
  /// @param halfWidth The half width of the base of the triangle,
  /// in psychoacoustic scale units.
  void AddTriangularFilter(float center, float halfWidth) const;

  mutable FloatPtr filter_bank_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction

#endif  // SRC_TRANSFORMS_FILTER_BANK_H_
