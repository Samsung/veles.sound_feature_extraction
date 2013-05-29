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

#include <mutex>
#include "src/formats/window_format.h"
#include "src/omp_transform_base.h"

struct ConvoluteHandle;

namespace SoundFeatureExtraction {
namespace Transforms {

class Autocorrelation
    : public OmpUniformFormatTransform<Formats::WindowFormatF> {
 public:
  TRANSFORM_INTRO("AutoCorrelation", "Find the cross-correlation of a signal "
                                     "with itself.")

  OMP_TRANSFORM_PARAMETERS()

  void Initialize() const noexcept;

 protected:
  virtual void OnFormatChanged();

  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void Do(const Formats::WindowF& in,
                  Formats::WindowF* out) const noexcept;

 private:
  struct SyncHandle {
    std::shared_ptr<ConvoluteHandle> handle;
    std::shared_ptr<std::mutex> mutex;
  };

  mutable std::vector<SyncHandle> correlationHandles_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_DIFFRECT_H_
