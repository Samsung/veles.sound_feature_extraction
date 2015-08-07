/*! @file peak_analysis.cc
 *  @brief Tests for sound_feature_extraction::transforms::PeakAnalysis.
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


#include "src/transforms/peak_analysis.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::PeakAnalysis;

class PeakAnalysisTest : public TransformTest<PeakAnalysis> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 4;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size - 1; i++) {
      (*Input)[0][i][0] = (i + 1) * 10;
      (*Input)[0][i][1] = 100;
    }
    (*Input)[0][Size - 1][0] = 10;
    (*Input)[0][Size - 1][1] = 101;
  }
};

TEST_F(PeakAnalysisTest, Do) {
  Do((*Input)[0], (*Output)[0]);
  ASSERT_FLOAT_EQ(10, (*Output)[0][0]);
  for (int i = 0; i < Size - 1; i++) {
    ASSERT_FLOAT_EQ(i + 1, (*Output)[0][i * 2 + 1]);
    ASSERT_NEAR(0.25f, (*Output)[0][i * 2 + 2], 0.001f);
  }
  ASSERT_FLOAT_EQ(1, (*Output)[0][(Size - 1) * 2 + 1]);
  ASSERT_NEAR(0.25f, (*Output)[0][(Size - 1) * 2 + 2], 0.002f);
}
