/*! @file delta.cc
 *  @brief Tests for sound_feature_extraction::transforms::Delta.
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


#include "src/transforms/delta.h"
#include "tests/transforms/transform_test.h"
#include <fftf/api.h>

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Delta;

class DeltaTest : public TransformTest<Delta> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 486;
    SetUpTransform(2, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i;
      (*Input)[1][i] = i + 1;
    }
  }
};

TEST_F(DeltaTest, Do) {
  ASSERT_EQ(2U, (*Output).Count());
  Do((*Input), &(*Output));
  for (int i = 0; i < Size; i++) {
    ASSERT_NEAR((*Output)[0][i], 1, 0.00001f) << i;
  }
}
/*
TEST_F(DeltaTest, DoRegression) {
  set_type(sound_feature_extraction::transforms::kDeltaTypeRegression);
  set_use_simd(false);
  Do((*Input), &(*Output));
  set_use_simd(true);
  auto output_simd = std::static_pointer_cast<Delta::OutBuffers>(
      CreateOutputBuffers(Output->Count()));
  Do((*Input), &(*output_simd));
  for (int i = 0; i < Size; i++) {
    ASSERT_NEAR((*Output)[0][i], (*output_simd)[0][i], 0.00001f) << i;
  }
}
*/
