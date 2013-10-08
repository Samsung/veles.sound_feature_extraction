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

#ifndef SRC_TRANSFORMS_DIFFRECT_H_
#define SRC_TRANSFORMS_DIFFRECT_H_

#include <vector>
#include "src/transforms/common.h"

typedef struct ConvolutionHandle CrossCorrelationHandle;

namespace std {
  class mutex;
}

namespace SoundFeatureExtraction {
namespace Transforms {

class Autocorrelation
    : public OmpUniformFormatTransform<Formats::RawFormatF> {
 public:
  TRANSFORM_INTRO("AutoCorrelation", "Find the cross-correlation of a signal "
                                     "with itself.")

  OMP_TRANSFORM_PARAMETERS()

  void Initialize() const noexcept override;

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in,
                  float* out) const noexcept override;

 private:
  struct SyncHandle {
    std::shared_ptr<CrossCorrelationHandle> handle;
    std::shared_ptr<std::mutex> mutex;
  };

  mutable std::vector<SyncHandle> correlationHandles_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_DIFFRECT_H_
