/*! @file wavelet_filter_bank.cc
 *  @brief Tests for src/primitives/wavelet_filter_bank.h.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <gtest/gtest.h>
#include "src/primitives/wavelet_filter_bank.h"

using SpeechFeatureExtraction::Primitives::WaveletFilterBank;
using SpeechFeatureExtraction::Primitives::
    WaveletTreeInvalidDescriptionException;
using SpeechFeatureExtraction::Primitives::
    WaveletTreeDescriptionParseException;

TEST(WaveletFilterBank, ValidateDescription) {
  std::vector<int> desc { 3, 3, 2, 2, 3, 3 };
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
  ASSERT_NO_THROW(WaveletFilterBank::ParseDescription("1 1"));
  ASSERT_NO_THROW(WaveletFilterBank::ParseDescription(" 1\t1 "));
  ASSERT_NO_THROW(WaveletFilterBank::ParseDescription("3 4 4 2 2 5 5 4 3"));
  ASSERT_THROW(WaveletFilterBank::ParseDescription("abc"),
               WaveletTreeDescriptionParseException);
  ASSERT_THROW(WaveletFilterBank::ParseDescription("1 1 abc"),
                 WaveletTreeDescriptionParseException);
  ASSERT_THROW(WaveletFilterBank::ParseDescription("1 -1"),
               WaveletTreeDescriptionParseException);
  ASSERT_THROW(WaveletFilterBank::ParseDescription("1 1 1"),
               WaveletTreeInvalidDescriptionException);
}

TEST(WaveletFilterBank, Apply) {
  WaveletFilterBank wfb(WAVELET_TYPE_DAUBECHIES, 8,
                        { 3, 4, 4, 2, 2, 5, 5, 4, 3 });

  float src[512];
  int length = sizeof(src) / sizeof(src[0]);
  float res[length];
  for (int i = 0; i < length; i++) {
    src[i] = i * (1 - 2 * (i % 2));
    res[i] = .0f;
  }

  wfb.Apply(src, length, res);

  for (int i = 0; i < length; i++) {
    ASSERT_NE(.0f, res[i]);
  }

  // 24 bands survival
  WaveletFilterBank wfb24(WAVELET_TYPE_DAUBECHIES, 8,
                          { 3, 3, 3,
                            4, 4, 4,
                            5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                            6, 6, 6, 6, 6, 6, 6, 6 });
  for (int i = 0; i < length; i++) {
    src[i] = i * (1 - 2 * (i % 2));
    res[i] = .0f;
  }
  wfb24.Apply(src, length, res);
}

TEST(WaveletFilterBank, ApplyNotAPowerOf2) {
  WaveletFilterBank wfb24(WAVELET_TYPE_DAUBECHIES, 8,
                          { 3, 3, 3,
                            4, 4, 4,
                            5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                            6, 6, 6, 6, 6, 6, 6, 6 });

  float src[320];
  int length = sizeof(src) / sizeof(src[0]);
  float res[length];

  for (int i = 0; i < length; i++) {
    src[i] = i * (1 - 2 * (i % 2));
    res[i] = .0f;
  }
  wfb24.Apply(src, length, res);
}

#include "tests/google/src/gtest_main.cc"
