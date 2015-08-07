/*! @file flux.cc
 *  @brief Tests for sound_feature_extraction::transforms::Flux.
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
#include "src/transforms/flux.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Flux;

class FluxTest : public TransformTest<Flux> {
 public:
  static const int Size;

  virtual void SetUp() {
    SetUpTransform(2, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i;
      (*Input)[1][i] = i + 1;
    }
  }
};

const int FluxTest::Size = 100;

#define EPSILON 0.0001f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(FluxTest, Do) {
  const float valid_res = sqrtf((2.f * Size - 1) / (6.f * Size * (Size - 1)));
  Do((*Input), &(*Output));
  float res = Do(false, (*Input)[1], Size, (*Input)[0]);
  ASSERT_NEAR(valid_res, res, valid_res / 10000);
  EXPECT_NEAR(res, (*Output)[0], res / 10000);
  EXPECT_NEAR(res, (*Output)[1], res / 10000);
}

const float extra_param[FluxTest::Size] = { 0.f };

#define CLASS_NAME FluxTest
#define ITER_COUNT 400000
#define NO_OUTPUT
#define EXTRA_PARAM extra_param
#include "tests/transforms/benchmark.inc"
