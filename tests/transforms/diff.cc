/*! @file diffrect.cc
 *  @brief Tests for sound_feature_extraction::transforms::Diff.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
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


#include "src/transforms/diff.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Diff;

class DiffTest : public TransformTest<Diff> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 486;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i % 3 - i % 2;
    }
    set_swt(1);
    Initialize();
  }
};

TEST_F(DiffTest, Do) {
  set_rectify(true);
  set_swt(0);
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size - 1; i++) {
    int result;
    switch (i % 6) {
      case 1:
      case 3:
        result = 2;
        break;
      default:
        result = 0;
        break;
    }
    ASSERT_FLOAT_EQ(result, (*Output)[0][i]) << i;
  }
  ASSERT_FLOAT_EQ(0, (*Output)[0][Size - 1]);
}

TEST_F(DiffTest, DoNoRectify) {
  set_rectify(false);
  set_swt(0);
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size - 1; i++) {
    int result;
    switch (i % 6) {
      case 1:
      case 3:
        result = 2;
        break;
      case 2:
        result = -3;
        break;
      case 5:
        result = -1;
        break;
      default:
        result = 0;
        break;
    }
    ASSERT_FLOAT_EQ(result, (*Output)[0][i]) << i;
  }
  ASSERT_FLOAT_EQ(-1, (*Output)[0][Size - 1]);
}

TEST_F(DiffTest, DoSWTRectify) {
  set_rectify(true);
  set_swt(1);
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    ASSERT_GE((*Output)[0][i], 0) << i;
  }
}

TEST_F(DiffTest, DoSWTNoRectify) {
  set_rectify(false);
  set_swt(2);
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    ASSERT_EQ((*Output)[0][i], (*Output)[0][i]) << i;
  }
}

#define CLASS_NAME DiffTest
#define ITER_COUNT 500000
#include "tests/transforms/benchmark.inc"
