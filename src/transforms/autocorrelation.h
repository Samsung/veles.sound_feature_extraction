/*! @file autocorrelation.h
 *  @brief Find the cross-correlation of a signal with itself.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 *
 *  @section License
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */

#ifndef SRC_TRANSFORMS_AUTOCORRELATION_H_
#define SRC_TRANSFORMS_AUTOCORRELATION_H_

#include <vector>
#include <mutex>
#include "src/transforms/common.h"

typedef struct ConvolutionHandle CrossCorrelationHandle;

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
    SyncHandle() : handle(nullptr, std::free) {
    }

    SyncHandle(const SyncHandle&) : handle(nullptr, std::free) {
    }

    std::shared_ptr<CrossCorrelationHandle> handle;
    std::mutex mutex;
  };

  mutable std::vector<SyncHandle> correlation_handles_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_AUTOCORRELATION_H_
