/*! @file autocorrelation.cc
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

#define __STDC_LIMIT_MACROS
#include "src/transforms/autocorrelation.h"
#include <simd/correlate.h>
#include <fftf/api.h>
#include <mutex>

namespace SoundFeatureExtraction {
namespace Transforms {

void Autocorrelation::Initialize() const {
  correlationHandles_.resize(MaxThreadsNumber());
  for (int i = 0; i < MaxThreadsNumber(); i++) {
    correlationHandles_[i].handle = std::shared_ptr<CrossCorrelationHandle>(
        new CrossCorrelationHandle(cross_correlate_initialize(
            inputFormat_->Size(), inputFormat_->Size())),
        [](CrossCorrelationHandle *ptr) {
          cross_correlate_finalize(*ptr);
          delete ptr;
        }
    );
    correlationHandles_[i].mutex = std::make_shared<std::mutex>();
  }
}

size_t Autocorrelation::OnFormatChanged(size_t buffersCount) {
  outputFormat_->SetSize(inputFormat_->Size() * 2 - 1);
  return buffersCount;
}

void Autocorrelation::Do(const float* in, float* out)
const noexcept {
  for (auto hp : correlationHandles_) {
    if (hp.mutex->try_lock()) {
      cross_correlate(*hp.handle, in, in, out);
      hp.mutex->unlock();
      break;
    }
  }
}

REGISTER_TRANSFORM(Autocorrelation);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
