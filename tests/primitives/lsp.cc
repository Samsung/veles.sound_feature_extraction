/*! @file energy.cc
 *  @brief Tests for src/primitives/energy.cc.
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
#include <gtest/gtest.h>
#include <simd/correlate.h>
#include <simd/memory.h>
#include <simd/arithmetic-inl.h>
#include "src/primitives/lsp.h"
#include "src/primitives/lpc.h"

TEST(LSP, lpc_to_lsp) {
  const int length = 16;
  float lpc[length], freq[length] = {0};

  lpc[0] = -1;
  for (int i = 1; i < length; i++) {
    lpc[i] = 0;
  }
  int roots = lpc_to_lsp(false, lpc, length, 16, 0.015625f, freq);
  ASSERT_EQ(length, roots);
  float valid_roots[] { 0.0395292342, 0.196349695, 0.39269948, 0.589048564,
                        0.78539896, 0.981747687, 1.17809725, 1.37444675,
                        1.57079637, 1.76714671, 1.96349549, 2.15984488,
                        2.3561945, 2.55254412, 2.7488935, 2.94524288 };
  for (int i = 0; i < length; i++) {
    ASSERT_NEAR(valid_roots[i], freq[i], 0.01f);
  }
  roots = lpc_to_lsp(true, lpc, length, 16, 0.015625f, freq);
  ASSERT_EQ(length, roots);
  for (int i = 0; i < length; i++) {
    ASSERT_NEAR(valid_roots[i], freq[i], 0.01f);
  }
}


#include "tests/google/src/gtest_main.cc"
