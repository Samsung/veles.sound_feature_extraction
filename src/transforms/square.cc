/*! @file square.cc
 *  @brief Square the input signal.
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

#include "src/transforms/square.h"
#include <simd/arithmetic-inl.h>

namespace sound_feature_extraction {
namespace transforms {

void Square::Do(const float* in, float* out) const noexcept {
  Do(use_simd(), in, output_format_->Size(), out);
}

void Square::Do(bool simd, const float* input, int length,
                float* output) noexcept {
  if (simd) {
    real_multiply_array(input, input, length, output);
  } else {
    real_multiply_array_na(input, input, length, output);
  }
}

void SquareInverse::Do(const float* in UNUSED,
                       float* out UNUSED) const noexcept {
  assert(false && "Not implemented yet");
}

REGISTER_TRANSFORM(Square);
REGISTER_TRANSFORM(SquareInverse);

}  // namespace transforms
}  // namespace sound_feature_extraction
