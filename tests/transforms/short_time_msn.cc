/*! @file short_time_msn.cc
 *  @brief Tests for sound_feature_extraction::transforms::ShortTimeMeanScaleNormalization.
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


#include "src/transforms/short_time_msn.h"
#include "tests/transforms/transform_test.h"
#include <fftf/api.h>

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::ShortTimeMeanScaleNormalization;

class ShortTimeMeanScaleNormalizationTest
    : public TransformTest<ShortTimeMeanScaleNormalization> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 486;
    set_length(5);
    SetUpTransform(10, Size, 18000);
    for (int k = 0; k < 10; k++) {
      for (int i = 0; i < Size; i++) {
        (*Input)[k][i] = i + k;
      }
    }
  }
};

TEST_F(ShortTimeMeanScaleNormalizationTest, Do) {
  Do((*Input), &(*Output));
  ASSERT_NEAR((*Output)[0][0], -0.5, 0.00001f);
  ASSERT_NEAR((*Output)[0][1], -0.5, 0.00001f);
  ASSERT_NEAR((*Output)[0][2], -0.5, 0.00001f);
  ASSERT_NEAR((*Output)[1][0], -0.16666f, 0.00001f);
  ASSERT_NEAR((*Output)[1][1], -0.16666f, 0.00001f);
  ASSERT_NEAR((*Output)[1][2], -0.16666f, 0.00001f);
  ASSERT_NEAR((*Output)[2][0], 0, 0.00001f);
  ASSERT_NEAR((*Output)[2][1], 0, 0.00001f);
  ASSERT_NEAR((*Output)[2][2], 0, 0.00001f);
  ASSERT_NEAR((*Output)[3][0], 0, 0.00001f);
  ASSERT_NEAR((*Output)[3][1], 0, 0.00001f);
  ASSERT_NEAR((*Output)[3][2], 0, 0.00001f);
  ASSERT_NEAR((*Output)[9][0], 0.5, 0.00001f);
  ASSERT_NEAR((*Output)[9][1], 0.5, 0.00001f);
  ASSERT_NEAR((*Output)[9][2], 0.5, 0.00001f);
}
