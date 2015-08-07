/*! @file centroid.cc
 *  @brief Tests for sound_feature_extraction::transforms::Centroid.
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
#include "src/transforms/mix_stereo.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormat16;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::MixStereo;

class MixStereoTest : public TransformTest<MixStereo> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 450;
    SetUpTransform(1, Size, 18000);
    for (int i = 0; i < Size; i += 2) {
      (*Input)[0][i] = i;
      (*Input)[0][i + 1] = -i * 2;
    }
  }
};

TEST_F(MixStereoTest, Do) {
  ASSERT_EQ(Size / 2U, output_format_->Size());
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < Size / 2; i++) {
    ASSERT_EQ(-i, (*Output)[0][i]);
  }
}
