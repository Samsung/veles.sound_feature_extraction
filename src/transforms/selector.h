/*! @file selector.h
 *  @brief Select the specified part of input.
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

#ifndef SRC_TRANSFORMS_SELECTOR_H_
#define SRC_TRANSFORMS_SELECTOR_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

enum class Anchor {
  kLeft,
  kRight
};

namespace internal {
constexpr const char* kAnchorLeftStr = "left";
constexpr const char* kAnchorRightStr = "right";
}

Anchor Parse(const std::string value, identity<Anchor>);

}  // namespace transforms
}  // namespace sound_feature_extraction

namespace std {
  using sound_feature_extraction::transforms::Anchor;

  inline string to_string(const Anchor& a) noexcept {
    return a == Anchor::kLeft?
        sound_feature_extraction::transforms::internal::kAnchorLeftStr :
        sound_feature_extraction::transforms::internal::kAnchorRightStr;
  }
}  // namespace std

namespace sound_feature_extraction {
namespace transforms {

class Selector : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  Selector();

  TRANSFORM_INTRO("Selector", "Selects the specified part of input.",
                  Selector)

  TP(length, int, kDefaultLength,
     "The resulting vector length. 0 means the length of the input.")
  TP(select, int, kDefaultSelect,
     "The length of picked up input values, The rest will be set to zero. "
     "0 means the length of the output.")
  TP(from, Anchor, kDefaultAnchor,
     "The anchor of the selection. Can be either \"left\" or \"right\".")

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override;

  virtual void Do(const float* in, float* out) const noexcept override;

  static constexpr int kDefaultLength = 0;
  static constexpr int kDefaultSelect = 0;
  static constexpr Anchor kDefaultAnchor = Anchor::kLeft;
};

}  // namespace transforms
}  // namespace sound_feature_extraction

#endif  // SRC_TRANSFORMS_SELECTOR_H_
