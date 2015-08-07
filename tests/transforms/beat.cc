/*! @file beat.cc
 *  @brief Tests for sound_feature_extraction::transforms::Beat.
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
#include "src/transforms/beat.h"
#include "tests/transforms/transform_test.h"
#include "tests/transforms/beat_test.inc"

using sound_feature_extraction::transforms::Beat;

class BeatTest : public TransformTest<Beat> {
 public:
  int Size;

  virtual void SetUp() {
    Size = sizeof(data_test) / sizeof(data_test[0]);
    SetUpTransform(1, Size, 3000);
    memcpy((*Input)[0], data_test, Size * sizeof(float));
  }
};

TEST_F(BeatTest, Do) {
  Do((*Input), &(*Output));
  ASSERT_NEAR(180.f, (*Output)[0][1][0], 5.f);
}

TEST_F(BeatTest, CombConvolve) {
  auto data_size = sizeof(data_conv) / sizeof(data_conv[0]);
  float out[data_size + 2001 - 1];
  CombConvolve(data_conv, data_size, 3, 1000, out);
  for (int i = 0; i < static_cast<int>(data_size) + 2000; i++) {
    ASSERT_NEAR(out[i], data_conv_result[i], 0.0001f) << "i = " << i;
  }
}
