/*! @file log.cc
 *  @brief Tests for sound_feature_extraction::transforms::Log.
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
#include "src/transforms/log.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::LogRaw;

class LogTest : public TransformTest<LogRaw> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 378;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = (i + Size / 2.0f) / Size;
    }
  }
};

#define EPSILON 0.000075f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

TEST_F(LogTest, Do) {
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    float log = (*Output)[0][i];
    float vlog = logf((i + Size / 2.0f) / Size + 1);
    ASSERT_EQF(vlog, log) << i;
  }
}

TEST_F(LogTest, DoScale) {
  set_scale(1000);
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    float log = (*Output)[0][i];
    float vlog = logf((i + Size / 2.0f) / Size * 1000 + 1);
    ASSERT_EQF(vlog, log) << i;
  }
}

TEST_F(LogTest, DoBare) {
  set_add1(false);
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    float log = (*Output)[0][i];
    float vlog = logf((i + Size / 2.0f) / Size);
    ASSERT_EQF(vlog, log) << i;
  }
}

#define CLASS_NAME LogTest
#include "tests/transforms/benchmark.inc"

