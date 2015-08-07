/*! @file window.cc
 *  @brief Tests for sound_feature_extraction::transforms::Window.
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


#include "src/transforms/window.h"
#include "tests/transforms/transform_test.h"
#include "src/transform_tree.h"
#include "src/transform_registry.h"
#include "src/formats/array_format.h"
#include "tests/speech_sample.inc"

using sound_feature_extraction::transforms::Window;

class WindowTest : public TransformTest<Window> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 514;
    SetUpTransform(1, Size, 16000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = i * (1 - 2 * (i % 2));
    }
  }
};

TEST_F(WindowTest, Do) {
  set_predft(false);
  Initialize();
  Do((*Input)[0], (*Output)[0]);
  float output_na[Size] __attribute__((aligned (32)));
  ApplyWindow(false, window_.get(), Size, (*Input)[0], output_na);
  ASSERT_EQ(0, memcmp((*Output)[0], output_na,
                      sizeof(float) * Size));
}

TEST_F(WindowTest, DoPreDft) {
  set_predft(true);
  Initialize();
  Do((*Input)[0], (*Output)[0]);
}
