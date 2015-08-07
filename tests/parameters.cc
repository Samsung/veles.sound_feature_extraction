/*! @file parameters.cc
 *  @brief Tests for src/parameters.h.
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

#include <gtest/gtest.h>
#include "src/parameterizable.h"

using sound_feature_extraction::Parameterizable;
using sound_feature_extraction::ParseParametersException;

TEST(Parameters, EmptyParse) {
  auto pp = Parameterizable::Parse("");
  ASSERT_EQ(0U, pp.size());
}

TEST(Parameters, Parse) {
  auto pp = Parameterizable::Parse(
      "one=56, two = some extra , \t three with spaces= xxx ");
  ASSERT_EQ(3U, pp.size());
  int size = 0;
  for (auto ppr : pp) {
    if (ppr.first == "one") {
       EXPECT_STREQ("56", ppr.second.c_str());
       size++;
    } else if (ppr.first == "two") {
       EXPECT_STREQ("some extra", ppr.second.c_str());
       size++;
    } else if (ppr.first == "three with spaces") {
       EXPECT_STREQ("xxx", ppr.second.c_str());
       size++;
    }
  }
  ASSERT_EQ(3, size);
}

TEST(Parameters, InvalidParse) {
  ASSERT_THROW({
    auto pp = Parameterizable::Parse("256");
  }, ParseParametersException);
}

#include "tests/google/src/gtest_main.cc"
