/*! @file wavelet_filter_bank.cc
 *  @brief Tests for src/primitives/wavelet_filter_bank.h.
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
#include "src/primitives/wavelet_filter_bank.h"

using sound_feature_extraction::primitives::WaveletFilterBank;
using sound_feature_extraction::TreeFingerprint;
using sound_feature_extraction::Parse;
using sound_feature_extraction::primitives::
    WaveletTreeInvalidDescriptionException;
using sound_feature_extraction::WaveletTreeDescriptionParseException;

TEST(WaveletFilterBank, ValidateDescription) {
  std::vector<int> desc { 3, 3, 2, 2, 3, 3 };  // NOLINT(*)
  ASSERT_NO_THROW(WaveletFilterBank::ValidateDescription(desc));
  desc = { 3, 3, 2, 3, 3, 2 };
  ASSERT_NO_THROW(WaveletFilterBank::ValidateDescription(desc));
  desc = { 3, 4, 4, 2, 2, 5, 5, 4, 3 };
  ASSERT_NO_THROW(WaveletFilterBank::ValidateDescription(desc));
  desc = { 1, 1 };
  ASSERT_NO_THROW(WaveletFilterBank::ValidateDescription(desc));
  desc.clear();
  ASSERT_THROW(WaveletFilterBank::ValidateDescription(desc),
               WaveletTreeInvalidDescriptionException);
  desc = { 1 };
  ASSERT_THROW(WaveletFilterBank::ValidateDescription(desc),
               WaveletTreeInvalidDescriptionException);
  desc = { 3, 3, 4, 4, 2, 2, 5, 5, 4, 3 };
  ASSERT_THROW(WaveletFilterBank::ValidateDescription(desc),
               WaveletTreeInvalidDescriptionException);
}

TEST(WaveletFilterBank, ParseDescription) {
  ASSERT_NO_THROW(Parse<TreeFingerprint>("1 1"));
  ASSERT_NO_THROW(Parse<TreeFingerprint>(" 1\t1 "));
  ASSERT_NO_THROW(Parse<TreeFingerprint>("3 4 4 2 2 5 5 4 3"));
  ASSERT_THROW(Parse<TreeFingerprint>("abc"),
               WaveletTreeDescriptionParseException);
  ASSERT_THROW(Parse<TreeFingerprint>("1 1 abc"),
                 WaveletTreeDescriptionParseException);
  ASSERT_THROW(Parse<TreeFingerprint>("1 -1"),
               WaveletTreeDescriptionParseException);
  auto desc = Parse<TreeFingerprint>("1 1 1");
  ASSERT_THROW(WaveletFilterBank::ValidateDescription(desc),
               WaveletTreeInvalidDescriptionException);
}

std::vector<std::pair<WaveletType, std::vector<int>>> Wavelets {
    { WAVELET_TYPE_DAUBECHIES, { 4, 6, 8, 12, 16 } },
    { WAVELET_TYPE_SYMLET, { 4, 6, 8, 12, 16 } },
    { WAVELET_TYPE_COIFLET, { 6, 12 } }
  };

TEST(WaveletFilterBank, Apply) {
  for (auto wp : Wavelets) {
    for (int order : wp.second) {
      WaveletFilterBank wfb(wp.first, order,
                            { 3, 4, 4, 2, 2, 5, 5, 4, 3 });

      float src[512];
      const int length = sizeof(src) / sizeof(src[0]);
      float res[length];  // NOLINT(runtime/arrays)
      for (int i = 0; i < length; i++) {
        src[i] = i * (1 - 2 * (i % 2));
        res[i] = 0.f;
      }

      wfb.Apply(src, length, res);

      // 24 bands survival
      WaveletFilterBank wfb24(wp.first, order,
                              { 3, 3, 3,
                                4, 4, 4,
                                5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                                6, 6, 6, 6, 6, 6, 6, 6 });
      for (int i = 0; i < length; i++) {
        src[i] = i * (1 - 2 * (i % 2));
        res[i] = 0.f;
      }
      wfb24.Apply(src, length, res);
    }
  }
}

TEST(WaveletFilterBank, ApplyNotAPowerOf2) {
  for (auto wp : Wavelets) {
    for (int order : wp.second) {
      WaveletFilterBank wfb24(wp.first, order,
                              { 3, 3, 3,
                                4, 4, 4,
                                5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                                6, 6, 6, 6, 6, 6, 6, 6 });

      float src[320];
      const int length = sizeof(src) / sizeof(src[0]);
      float res[length];  // NOLINT(runtime/arrays)

      for (int i = 0; i < length; i++) {
        src[i] = i * (1 - 2 * (i % 2));
        res[i] = 0.f;
      }
      wfb24.Apply(src, length, res);
    }
  }
}

#include "tests/google/src/gtest_main.cc"
