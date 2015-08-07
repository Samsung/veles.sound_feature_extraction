/*! @file frequency_bands.cc
 *  @brief Tests for sound_feature_extraction::transforms::FrequencyBands.
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


#include "src/transforms/frequency_bands.h"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::FrequencyBands;

class FrequencyBandsTest : public TransformTest<FrequencyBands> {
 public:
  int Size;
  int Buffers;

  virtual void SetUp() {
    Size = 512;
    Buffers = 2;
    set_number(Buffers);
    SetUpTransform(Buffers, Size, 16000);
    for (int i = 0; i < Size; i++) {
      for (int j = 0; j < Buffers; j++) {
        (*Input)[j][i] = sinf(i / 10.f);
      }
    }
  }
};

TEST_F(FrequencyBandsTest, Do) {
  ASSERT_EQ(static_cast<size_t>(Size), input_format_->Size());
  ASSERT_EQ(static_cast<size_t>(Size), output_format_->Size());
  Do((*Input), &(*Output));
}

TEST_F(FrequencyBandsTest, DoWithBands) {
  set_bands("2000 3000 5000");
  Initialize();
  Do((*Input), &(*Output));
}

TEST_F(FrequencyBandsTest, TooBigBands) {
  set_bands("2000 3000 18000");
  Initialize();
  ASSERT_EQ(3U, filters().size());
}

TEST_F(FrequencyBandsTest, InvalidBands) {
  ASSERT_THROW({
    set_bands("2000 15000 8000");
  }, sound_feature_extraction::InvalidParameterValueException);
}
