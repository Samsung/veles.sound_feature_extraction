/*! @file reorder.h
 *  @brief Reorders the values in the array.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_REORDER_H_
#define SRC_TRANSFORMS_REORDER_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

enum class ReorderAlgorithm {
  kChroma
};

namespace internal {
constexpr const char* kReorderAlgorithmChromaStr = "chroma";
}

ReorderAlgorithm Parse(const std::string& value, identity<ReorderAlgorithm>);

}  // namespace transforms
}  // namespace sound_feature_extraction

namespace std {
  using sound_feature_extraction::transforms::ReorderAlgorithm;

  inline string to_string(const ReorderAlgorithm& value) noexcept {
    switch (value) {
      case ReorderAlgorithm::kChroma:
        return sound_feature_extraction::transforms::internal::
            kReorderAlgorithmChromaStr;
    }
    return "";
  }
}

namespace sound_feature_extraction {
namespace transforms {

class Reorder : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  Reorder();

  TRANSFORM_INTRO("Reorder", "Reorders the values in the array according to "
                             "the specified algorithm.",
                  Reorder)

  TP(algorithm, ReorderAlgorithm, kDefaultAlgorithm,
     "The way to reorder the values.")


 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override final;

  virtual void Do(const float* in, float* out) const noexcept override;

  void DoChroma(const float* in, float* out) const noexcept;

  static constexpr ReorderAlgorithm kDefaultAlgorithm =
      ReorderAlgorithm::kChroma;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_REORDER_H_
