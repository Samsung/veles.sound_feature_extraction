/*! @file reorder.cc
 *  @brief Reorders the values in the array.
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

#include "src/transforms/reorder.h"

namespace sound_feature_extraction {
namespace transforms {

ReorderAlgorithm Parse(const std::string& value, identity<ReorderAlgorithm>) {
  static const std::unordered_map<std::string, ReorderAlgorithm> map {
    { internal::kReorderAlgorithmChromaStr, ReorderAlgorithm::kChroma }
  };
  auto rait = map.find(value);
  if (rait == map.end()) {
    throw InvalidParameterValueException();
  }
  return rait->second;
}

constexpr ReorderAlgorithm Reorder::kDefaultAlgorithm;

Reorder::Reorder() : algorithm_(kDefaultAlgorithm) {
}

ALWAYS_VALID_TP(Reorder, algorithm)

size_t Reorder::OnFormatChanged(size_t buffersCount) {
  switch (algorithm_) {
    case ReorderAlgorithm::kChroma:
      output_format_->SetSize(
          input_format_->Size() - input_format_->Size() % 12);
      break;
  }
  return buffersCount;
}

void Reorder::Do(const float* in, float* out) const noexcept {
  switch (algorithm_) {
    case ReorderAlgorithm::kChroma:
      DoChroma(in, out);
      break;
  }
}

void Reorder::DoChroma(const float* in, float* out) const noexcept {
  int step = output_format_->Size() / 12;
  for (int i = 0; i < step; i++) {
    for (int j = 0; j < 12; j++) {
      out[j * step + i] = in[i * 12];
    }
  }
}

RTP(Reorder, algorithm)
REGISTER_TRANSFORM(Reorder);

}  // namespace transforms
}  // namespace sound_feature_extraction
