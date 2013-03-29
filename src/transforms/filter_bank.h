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

#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class FilterBank
    : public UniformFormatTransform<Formats::WindowFormatF> {
 public:
  FilterBank();

  TRANSFORM_INTRO("FilterBank",
                  "Converts the signal to the selected psychoacoustic scale "
                  "(default is mel).")

  TRANSFORM_PARAMETERS(
      TP("type", "The type of the scale. Supported values are \"linear\","
                 "\"mel\" and \"bark\".",
         "mel")
      TP("number", "The number of triangular filters.",
         std::to_string(kDefaultLength))
      TP("frequency_min", "Minimal frequency of the filter bank.",
         std::to_string(kDefaultMinFrequency))
      TP("frequency_max", "Maximal frequency of the filter bank.",
         std::to_string(kDefaultMaxFrequency))
  )

  virtual bool HasInverse() const noexcept;

  virtual void Initialize() const noexcept;

 protected:
  enum ScaleType {
    SCALE_TYPE_LINEAR,
    SCALE_TYPE_MEL,
    SCALE_TYPE_BARK
  };

  static const std::unordered_map<std::string, ScaleType> kScaleTypeMap;
  static const int kDefaultLength;
  static const int kDefaultMinFrequency;
  static const int kDefaultMaxFrequency;

  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers)
  const noexcept;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                  BuffersBase<Formats::WindowF>* out) const noexcept;


  static float LinearToScale(ScaleType type, float freq);
  static float ScaleToLinear(ScaleType type, float value);

  ScaleType type_;
  size_t length_;
  int minFreq_;
  int maxFreq_;
  mutable std::shared_ptr<float> filterBank_;

 private:
  /// @brief Adds a triangular filter to the filter bank.
  /// @param center The value of the peak of the triangle,
  /// in psychoacoustic scale units.
  /// @param halfWidth The half width of the base of the triangle,
  /// in psychoacoustic scale units.
  void AddTriangularFilter(float center, float halfWidth) const;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_FILTER_BANK_H_
