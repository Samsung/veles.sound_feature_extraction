/*! @file rdft.cc
 *  @brief Tests for sound_feature_extraction::transforms::RDFT.
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

#include "src/transform_tree.h"
#include "src/transforms/rdft.h"
#include "tests/speech_sample.inc"
#include "tests/transforms/transform_test.h"

using sound_feature_extraction::formats::ArrayFormatF;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::transforms::RDFT;
using sound_feature_extraction::transforms::RDFTInverse;
using sound_feature_extraction::TransformTree;
using sound_feature_extraction::BuffersBase;

class RDFTTest : public TransformTest<RDFT> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 512;
    SetUpTransform(1, Size, 16000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = (i - Size / 2.0f) / Size;
    }
  }
};

class RDFTInverseTest : public TransformTest<RDFTInverse> {
 public:
  int Size;

  virtual void SetUp() {
    Size = 514;
    SetUpTransform(1, Size, 16000);
    for (int i = 0; i < Size; i++) {
      (*Input)[0][i] = (i - Size / 2.0f) / Size;
    }
  }
};

TEST_F(RDFTTest, Do) {
  Do((*Input), &(*Output));
}

TEST_F(RDFTInverseTest, Do) {
  Do((*Input), &(*Output));
}

TEST(RDFT, Multiple) {
  TransformTree tt( { 48000, 16000 } );  // NOLINT(*)
  tt.set_validate_after_each_transform(true);
  // We have to apply FilterBank twice since Energy results in
  // squared magnitude
  tt.AddFeature("3RDFT", { { "Window", "length=500, type=rectangular" },
      { "RDFT", "" }, { "IRDFT", "" }, { "RDFT", "" }, { "IRDFT", "" },
      { "RDFT", "" }, { "IRDFT", "" }
  });
  std::unique_ptr<int16_t[]> buffers(new int16_t[48000]);
  memcpy(buffers.get(), data, sizeof(data));
  tt.PrepareForExecution();
  auto res = tt.Execute(buffers.get());
  ASSERT_EQ(1U, res.size());
  res["3RDFT"]->Validate();
}

