/*! @file stretch.cc
 *  @brief Tests for sound_feature_extraction::transforms::Stretch.
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


#include "src/transforms/stretch.h"
#include <cmath>
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Stretch;

class StretchTest : public TransformTest<Stretch> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 12;
    set_center(false);
    Reinitialize();
  }

  void Reinitialize() {
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i;
    }
  }
};

TEST_F(StretchTest, Do) {
  set_factor(1);
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    ASSERT_FLOAT_EQ((*Input)[0][i], (*Output)[0][i]) << i;
  }
  set_factor(2);
  Reinitialize();
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size * 2; i++) {
    ASSERT_FLOAT_EQ((*Input)[0][i / 2], (*Output)[0][i]) << i;
  }
  set_factor(10);
  Reinitialize();
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size * 10; i++) {
    ASSERT_FLOAT_EQ((*Input)[0][i / 10], (*Output)[0][i]) << i;
  }
  set_factor(250);
  Reinitialize();
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size * 250; i++) {
    ASSERT_FLOAT_EQ((*Input)[0][i / 250], (*Output)[0][i]) << i;
  }
}

TEST_F(StretchTest, DoCenter) {
  set_center(true);
  set_factor(1);
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    ASSERT_FLOAT_EQ((*Input)[0][i], (*Output)[0][i]) << i;
  }
  set_factor(2);
  Reinitialize();
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size * 2; i++) {
    ASSERT_FLOAT_EQ((*Input)[0][i / 2], (*Output)[0][i]) << i;
  }
  set_factor(250);
  Reinitialize();
  Do((*Input)[0], (*Output)[0]);
  float ilen = 250.f * Size / (Size + 1);
  for (int i = 0; i < Size; i++) {
    for (int j = i * ilen + ilen / 2;
        j < static_cast<int>((i + 1) * ilen + ilen / 2); j++) {
      ASSERT_FLOAT_EQ((*Input)[0][i], (*Output)[0][j]) << j;
    }
  }
  for (int i = 0; i < ilen / 2; i++) {
    ASSERT_FLOAT_EQ((*Input)[0][0], (*Output)[0][i]) << i;
  }
  for (int i = Size * 250 - ilen / 2; i < Size * 250; i++) {
    ASSERT_FLOAT_EQ((*Input)[0][Size - 1], (*Output)[0][i]) << i;
  }
}

TEST_F(StretchTest, CRPCase) {
  set_center(true);
  set_factor(9.31677);
  Size = 322;
  Reinitialize();
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size; i++) {
    ASSERT_FLOAT_EQ((*Input)[0][i],
                    (*Output)[0][static_cast<int>(i * factor())]) << i;
  }
}
