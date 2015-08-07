/*! @file lpc_cc.cc
 *  @brief Tests for sound_feature_extraction::transforms::LPC2CC.
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
#include "src/transforms/lpc_cc.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::LPC2CC;

class LPC2CCTest : public TransformTest<LPC2CC> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 10;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i + 2;
    }
  }
};

TEST_F(LPC2CCTest, Do) {
  set_size(Size);
  Do((*Input)[0], (*Output)[0]);
  const float valid_cc[] = {
     0.693147180559945,
    -3,
     0.500000000000000,
    -2,
     1.25000000000000,
    -2.60000000000000,
     2.66666666666667,
    -4.42857142857143,
     5.62500000000000,
    -8.66666666666666,
     24.1000000000000,
    -41.2727272727273,
     61.6666666666666,
    -98.2307692307692,
     153.071428571429
  };
  for (int i = 0; i < Size; i++) {
    ASSERT_NEAR(valid_cc[i], (*Output)[0][i], fabsf(valid_cc[i] / 100000));
  }
}
