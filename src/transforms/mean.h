/*! @file mean.h
 *  @brief Arithmetic and geometric means calculation.
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

#ifndef SRC_TRANSFORMS_MEAN_H_
#define SRC_TRANSFORMS_MEAN_H_

#include <set>
#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

enum MeanType {
  kMeanTypeArithmetic = 0,
  kMeanTypeGeometric,
  kMeanTypeCount
};

namespace internal {
constexpr const char* kMeanTypeArithmeticStr = "arithmetic";
constexpr const char* kMeanTypeGeometricStr = "geometric";
}

std::set<MeanType> Parse(const std::string& value,
                         identity<std::set<MeanType>>);

}  // namespace transforms
}  // namespace sound_feature_extraction

namespace std {
  inline string to_string(
      const set<sound_feature_extraction::transforms::MeanType>& value)
      noexcept {
    string res;
    for (auto type : value) {
      switch (type) {
        case sound_feature_extraction::transforms::kMeanTypeArithmetic:
          res += sound_feature_extraction::transforms::internal::
              kMeanTypeArithmeticStr;
          res += " ";
          break;
        case sound_feature_extraction::transforms::kMeanTypeGeometric:
          res += sound_feature_extraction::transforms::internal::
              kMeanTypeGeometricStr;
          res += " ";
          break;
        default:
          break;
      }
    }
    return res.empty()? "" : res.substr(0, res.size() - 1);
  }
}  // namespace std

namespace sound_feature_extraction {
namespace transforms {

class Mean
    : public OmpTransformBase<formats::ArrayFormatF,
                              formats::SingleFormat<
                                  formats::FixedArray<kMeanTypeCount>>> {
 public:
  Mean();

  TRANSFORM_INTRO("Mean", "Window means calculation.", Mean)

  TP(types, std::set<MeanType>, kDefaultMeanTypes(),
     "Mean types to calculate (names separated with spaces).")

 protected:
  virtual void Do(const float* in,
                  formats::FixedArray<kMeanTypeCount>* out)
      const noexcept override;

  static float Do(bool simd, const float* input, size_t length,
                  MeanType type) noexcept;

  static std::set<MeanType> kDefaultMeanTypes() noexcept {
    return { kMeanTypeArithmetic };
  }
};

}  // namespace transforms
}  // namespace sound_feature_extraction

#endif  // SRC_TRANSFORMS_MEAN_H_
