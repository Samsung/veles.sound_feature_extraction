/*! @file speech_features.cc
 *  @brief Tests for src/speech_features.cc.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "src/speech_features.h"

using SpeechFeatureExtraction::Features::Parse;

TEST(Features, Parse) {
  std::vector<std::string> lines = { "MFCC[DFT, MelFreq(size=16), Log10, DCT(engine=Kiss)]" };
  auto result = Parse(lines);
  ASSERT_EQ(result.size(), 1);
}

#include "tests/google/src/gtest_main.cc"
