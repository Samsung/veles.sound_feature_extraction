/*! @file highpass_filter.cc
 *  @brief Tests for sound_feature_extraction::transforms::HighpassFilter.
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

#include <fstream>
#include "src/transforms/highpass_filter.h"
#include "tests/transforms/transform_test.h"
#include "src/primitives/window.h"
#include "tests/transforms/pass_signal.inc"

#define EPSILON 0.0001f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

using sound_feature_extraction::transforms::HighpassFilter;

class HighpassFilterTest : public TransformTest<HighpassFilter> {
 public:
  virtual void SetUp() {
    set_frequency(2000);
    Size = sizeof(data_signal) / sizeof(data_signal[0]);
    SetUpTransform(1, 10 * Size, 22050);
    for (int i = 0; i < 10; i++) {
      memcpy((*Input)[0] + i * Size, data_signal, sizeof(data_signal));
    }
  }

  size_t Size;
};

TEST_F(HighpassFilterTest, CalculateHighpassFilter) {
  Do((*Input)[0], (*Output)[0]);

  std::ofstream fs("/tmp/out.txt", std::ios::out);
  fs << '[';
  for (int i = 0; i < 10000; i++) {
    fs << (*Output)[0][i] << ", ";
  }
  fs << "]\n";
}
