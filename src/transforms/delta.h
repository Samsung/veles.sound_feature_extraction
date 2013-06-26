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

#include "src/formats/raw_format.h"
#include "src/transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class Delta
    : public UniformFormatTransform<Formats::RawFormatF> {
 public:
  Delta();

  TRANSFORM_INTRO("Delta", "Get the difference between values of adjacent "
                           "windows.")

  TRANSFORM_PARAMETERS(
      TP("type", "The algorithm to calculate the deltas with. Allowed values "
                 "are \"simple\" and \"regression\".", "simple")
      TP("rlength", "The linear regression window length. Only odd values "
                    " greater than 1 are accepted.",
         std::to_string(kDefaultRegressionLength))
  )

 protected:
  enum Type {
    kTypeSimple,
    kTypeRegression
  };

  static const std::unordered_map<std::string, Type> kTypesMap;
  static const int kDefaultRegressionLength = 5;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*>* out) const noexcept override;

  static void DoSimple(bool simd, const float* prev, const float* cur,
                       size_t length, float* res) noexcept;

  static void DoRegression(bool simd, const BuffersBase<float*>& in,
                           int rstep, int i, float norm, int windowSize,
                           BuffersBase<float*>* out) noexcept;

 private:
  Type type_;
  int rlength_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_DELTA_H_
