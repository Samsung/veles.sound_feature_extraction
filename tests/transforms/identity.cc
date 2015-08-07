/*! @file identity.cc
 *  @brief Tests for sound_feature_extraction::transforms::Identity.
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


#include "src/transforms/identity.h"
#include "src/formats/array_format.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Identity;

class IdentityTest : public Identity,
                     public testing::Test {
 public:
  virtual void SetUp() {
    SetInputFormat(std::make_shared<ArrayFormatF>(1000, 22050), 3);
    Input = std::make_shared<BuffersBase<float*>>(
        std::static_pointer_cast<ArrayFormatF>(input_format_), 3);
    Output = std::static_pointer_cast<BuffersBase<float*>>(
        CreateOutputBuffers(3));
    Initialize();

    for (int j = 0; j < 3; j++) {
      for (int i = 0; i < 1000; i++) {
        (*Input)[j][i] = i;
      }
    }
  }

 protected:
  std::shared_ptr<BuffersBase<float*>> Input;
  std::shared_ptr<BuffersBase<float*>> Output;
};

TEST_F(IdentityTest, Do) {
  Do((*Input), Output.get());
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 1000; i++) {
      ASSERT_FLOAT_EQ(i, (*Output)[j][i]);
    }
  }
}
