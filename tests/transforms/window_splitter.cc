/*! @file window_splitter.cc
 *  @brief New file description.
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

#include "tests/transforms/transform_test.h"
#include "src/transforms/window_splitter.h"

using sound_feature_extraction::transforms::WindowSplitterF;
using sound_feature_extraction::transforms::WindowSplitterFInverse;

class WindowSplitterTest : public TransformTest<WindowSplitterF> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 512 + 205;
    SetUpTransform(10, Size, 16000);
    for (int j = 0; j < 10; j++) {
      for (int i = 0; i < Size; i++) {
        (*Input)[j][i] = i * (1 - 2 * (i % 2));
      }
    }
  }
};

class WindowSplitterInverseTest : public TransformTest<WindowSplitterFInverse> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 512 + 205;
    SetUpTransform(10, Size, 16000);
    for (int j = 0; j < 10; j++) {
      for (int i = 0; i < Size; i++) {
        (*Input)[j][i] = i * (1 - 2 * (i % 2));
      }
    }
  }
};

TEST_F(WindowSplitterTest, Do) {
  Do(*Input, Output.get());
  ASSERT_EQ(Input->Count() * 2, Output->Count());
  ASSERT_EQ(512U, output_format_->Size());
}

TEST_F(WindowSplitterInverseTest, DoInterleaved) {
  set_interleaved(true);
  set_step(309);
  set_count(2);
  RecreateOutputBuffers();
  ASSERT_EQ(2U, Output->Count());
  ASSERT_EQ(1953U, output_format_->Size());
  Do(*Input, Output.get());
}
