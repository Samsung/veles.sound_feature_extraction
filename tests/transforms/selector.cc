/*! @file selector.cc
 *  @brief Tests for sound_feature_extraction::transforms::Selector.
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

#include "src/transforms/selector.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::Selector;

class SelectorTest : public TransformTest<Selector> {
 public:
  int Size = 512;

  virtual void SetUp() {
  }
};

TEST_F(SelectorTest, Do) {
  set_from(sound_feature_extraction::transforms::Anchor::kLeft);
  set_length(6);
  SetUpTransform(1, Size, 16000);
  for (int i = 0; i < Size; i++) {
    (*Input)[0][i] = i;
  }
  ASSERT_EQ(6U, output_format_->Size());
  Do((*Input)[0], (*Output)[0]);
  ASSERT_EQ(0, memcmp((*Input)[0],
                      (*Output)[0],
                      6 * sizeof(float)));  // NOLINT(*)
  set_from(sound_feature_extraction::transforms::Anchor::kRight);
  Do((*Input)[0], (*Output)[0]);
  ASSERT_EQ(0, memcmp((*Input)[0] + 512 - 6,
                      (*Output)[0],
                      6 * sizeof(float)));  // NOLINT(*)
}

TEST_F(SelectorTest, DoSelect) {
  set_from(sound_feature_extraction::transforms::Anchor::kLeft);
  set_length(120);
  set_select(80);
  SetUpTransform(1, Size, 16000);
  for (int i = 0; i < Size; i++) {
    (*Input)[0][i] = i;
  }
  ASSERT_EQ(120U, output_format_->Size());
  Do((*Input)[0], (*Output)[0]);
  ASSERT_EQ(0, memcmp((*Input)[0],
                      (*Output)[0],
                      80 * sizeof(float)));  // NOLINT(*)
  for (int i = 80; i < 120; i++) {
    ASSERT_FLOAT_EQ(0, (*Output)[0][i]) << i;
  }
  set_from(sound_feature_extraction::transforms::Anchor::kRight);
  Do((*Input)[0], (*Output)[0]);
  ASSERT_EQ(0, memcmp((*Input)[0] + 512 - 80,
                      (*Output)[0] + 40,
                      80 * sizeof(float)));  // NOLINT(*)
  for (int i = 0; i < 40; i++) {
    ASSERT_FLOAT_EQ(0, (*Output)[0][i]) << i;
  }
}
