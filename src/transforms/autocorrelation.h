/*! @file autocorrelation.h
 *  @brief Find the cross-correlation of a signal with itself.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_AUTOCORRELATION_H_
#define SRC_TRANSFORMS_AUTOCORRELATION_H_

#include <vector>
#include "src/transforms/common.h"

typedef struct ConvolutionHandle CrossCorrelationHandle;

namespace std {
  class mutex;
}

namespace sound_feature_extraction {
namespace transforms {

class Autocorrelation
    : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  Autocorrelation();

  TRANSFORM_INTRO("Autocorrelation",
                  "Find the cross-correlation of a signal with itself.",
                  Autocorrelation)

  TP(normalize, bool, kDefaultNormalize,
     "Calculate normalized autocorrelation by dividing each "
     "value by lag 0 result (squared signal sum).")

  void Initialize() const override;

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in, float* out) const noexcept override;

  static constexpr bool kDefaultNormalize = false;

 private:
  struct SyncHandle {
    std::shared_ptr<CrossCorrelationHandle> handle;
    std::shared_ptr<std::mutex> mutex;
  };

  mutable std::vector<SyncHandle> correlation_handles_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_AUTOCORRELATION_H_
