/*! @file delta.h
 *  @brief New file description.
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

#ifndef SRC_TRANSFORMS_DELTA_H_
#define SRC_TRANSFORMS_DELTA_H_

#include "src/formats/array_format.h"
#include "src/transform_base.h"

namespace sound_feature_extraction {
namespace transforms {

enum class DeltaType {
  kSimple,
  kRegression
};

namespace internal {
constexpr const char* kDeltaTypeSimpleStr = "simple";
constexpr const char* kDeltaTypeRegressionStr = "regression";
}

DeltaType Parse(const std::string& value, identity<DeltaType>);

}  // namespace transforms
}  // namespace sound_feature_extraction

namespace std {
  using sound_feature_extraction::transforms::DeltaType;

  inline string
  to_string(const DeltaType& value) noexcept {
    switch (value) {
      case DeltaType::kSimple:
        return sound_feature_extraction::transforms::internal::
            kDeltaTypeSimpleStr;
      case DeltaType::kRegression:
        return sound_feature_extraction::transforms::internal::
            kDeltaTypeRegressionStr;
    }
    return "";
  }
}  // namespace std

namespace sound_feature_extraction {
namespace transforms {

class Delta : public UniformFormatTransform<formats::ArrayFormatF> {
 public:
  Delta();

  TRANSFORM_INTRO("Delta", "Get the difference between values of adjacent "
                           "windows.",
                  Delta)

      TP(type, DeltaType, kDefaultDeltaType,
         "The algorithm to calculate the deltas. Allowed values are "
         "\"simple\" and \"regression\".")
      TP(rlength, int, kDefaultRegressionLength,
         "The linear regression window length. Only odd values "
         " greater than 1 are accepted.")

 protected:
  static constexpr DeltaType kDefaultDeltaType = DeltaType::kSimple;
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

#endif  // SRC_TRANSFORMS_DELTA_H_
