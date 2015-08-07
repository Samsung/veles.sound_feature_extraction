/*! @file filter_bank.cc
 *  @brief Tests for sound_feature_extraction::transforms::FilterBank.
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



#include "src/transforms/filter_bank.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::FilterBank;
using sound_feature_extraction::transforms::ScaleType;

class FilterBankTest : public TransformTest<FilterBank> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 257;
    set_number(8);
    set_squared(true);
    SetUpTransform(1, Size, 16000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = 100;
    }
  }
};

#define EPSILON 0.000075f

#define ASSERT_EQF(a, b) ASSERT_NEAR(a, b, EPSILON)

static const float kValidFilter5[257] {
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000139,
  0.003139, 0.009963, 0.020487, 0.034594, 0.052171, 0.073111, 0.097307,
  0.124661, 0.155074, 0.188453,
  0.224709, 0.263755, 0.305507, 0.349884, 0.396809, 0.446206, 0.498002,
  0.552129, 0.608517, 0.667103,
  0.727822, 0.790613, 0.855417, 0.922177, 1.000000, 0.940479, 0.874729,
  0.811884, 0.751875, 0.694638,
  0.640107, 0.588221, 0.538919, 0.492143, 0.447836, 0.405943, 0.366410,
  0.329185, 0.294216, 0.261454,
  0.230852, 0.202363, 0.175941, 0.151541, 0.129122, 0.108640, 0.090055,
  0.073327, 0.058417, 0.045288,
  0.033902, 0.024225, 0.016220, 0.009854, 0.005094, 0.001906, 0.000261,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000
};

TEST_F(FilterBankTest, Do) {
  for (int i = 0; i < Size; i++) {
    auto& filter = filter_bank()[5];
    if (i < filter.begin || i > filter.end) {
      ASSERT_FLOAT_EQ(kValidFilter5[i], 0) << i;
      continue;
    }
    ASSERT_NEAR(kValidFilter5[i], filter.data[i - filter.begin], 0.000001) << i;
  }
  Do((*Input)[0], (*Output)[0]);
  for (int i = 0; i < number(); i++) {
    auto& filter = filter_bank()[i];
    float sum = 0;
    for (int j = filter.begin; j <= filter.end; j++) {
      float val = (*Input)[0][j] * filter.data[j - filter.begin];
      sum += val * val;
    }
    ASSERT_NEAR(sum, (*Output)[0][i], sum / 100000) << i;
  }
}


class ScaleTest : public ::testing::TestWithParam<std::tuple<ScaleType, float>>,
                  public FilterBank {
 protected:
  virtual void SetUp() override {
  }
};

TEST_P(ScaleTest, Identity) {
  float to = LinearToScale(std::get<0>(GetParam()), std::get<1>(GetParam()));
  float from2 = ScaleToLinear(std::get<0>(GetParam()), to);
  ASSERT_NEAR(std::get<1>(GetParam()), from2, std::get<1>(GetParam()) / 30000);
}

INSTANTIATE_TEST_CASE_P(
    Identity, ScaleTest,
    ::testing::Combine(
    ::testing::Values(ScaleType::kLinear, ScaleType::kMel, ScaleType::kBark,
                      ScaleType::kMidi),
    ::testing::Values(7.946362749, 29.1352350949, 100, 200, 300, 500, 520, 800,
                      1000, 3364, 9000, 14644))
    );
