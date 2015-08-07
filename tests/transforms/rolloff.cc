/*! @file rolloff.cc
 *  @brief Tests for sound_feature_extraction::transforms::Rolloff.
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

#include <cmath>
#include "src/transforms/rolloff.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Rolloff;

class RolloffTest : public TransformTest<Rolloff> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 450;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      // Always liked exotic functions
      (*Input)[0][i] = fabs(sinf(i * i) + i * cosf(i));
    }
  }
};

#define EPSILON 0.0025f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(RolloffTest, Do) {
  set_ratio(0.8);
  Do((*Input)[0], &(*Output)[0]);
  float res = Do(false, (*Input)[0], Size, 0.8f);
  res /= input_format_->Duration();
  ASSERT_EQF(res, (*Output)[0]);
}

#define CLASS_NAME RolloffTest
#define ITER_COUNT 400000
#define NO_OUTPUT
#define EXTRA_PARAM 0.85f
#include "tests/transforms/benchmark.inc"
