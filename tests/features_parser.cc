/*! @file features_parser.cc
 *  @brief Tests for src/features_parser.cc.
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
#include <string>
#include <vector>
#include "src/features_parser.h"

using SoundFeatureExtraction::Features::Parse;

TEST(Features, Parse) {
  std::vector<std::string> lines = {
      "MFCC[Window(length=25, step=10), DFT, MelFreq(size=16), "
          "Log10, DCT(engine=Kiss)]",
      "PLP[Window(length=25, step=10), IntensityLoudness(direction=i2l), "
          "IDFT, AutoRegressiveAnalysis(), LPCtoCC]",
      "SBC [Window(length = 32), RDFT]",
      "WPP[Window(length=512, type=rectangular), DWPT, "
          "SubbandEnergy, Log, DWPT(order=4, tree=1 2 3 3)]"
  };
  auto result = Parse(lines);
  ASSERT_EQ(4U, result.size());
  auto it = result.find("PLP");
  EXPECT_NE(result.end(), it);
  EXPECT_EQ(5U, it->second.size());
  auto tit = it->second.begin();
  EXPECT_STREQ("Window", tit->first.c_str());
  EXPECT_STREQ("length=25, step=10", tit++->second.c_str());
  EXPECT_STREQ("IntensityLoudness", tit->first.c_str());
  EXPECT_STREQ("direction=i2l", tit++->second.c_str());
  EXPECT_STREQ("IDFT", tit->first.c_str());
  EXPECT_STREQ("", tit++->second.c_str());
  EXPECT_STREQ("AutoRegressiveAnalysis", tit->first.c_str());
  EXPECT_STREQ("", tit++->second.c_str());
  EXPECT_STREQ("LPCtoCC", tit->first.c_str());
  EXPECT_STREQ("", tit++->second.c_str());
  it = result.find("MFCC");
  EXPECT_NE(result.end(), it);
  EXPECT_EQ(5U, it->second.size());
  tit = it->second.begin();
  EXPECT_STREQ("Window", tit->first.c_str());
  EXPECT_STREQ("length=25, step=10", tit++->second.c_str());
  EXPECT_STREQ("DFT", tit->first.c_str());
  EXPECT_STREQ("", tit++->second.c_str());
  EXPECT_STREQ("MelFreq", tit->first.c_str());
  EXPECT_STREQ("size=16", tit++->second.c_str());
  EXPECT_STREQ("Log10", tit->first.c_str());
  EXPECT_STREQ("", tit++->second.c_str());
  EXPECT_STREQ("DCT", tit->first.c_str());
  EXPECT_STREQ("engine=Kiss", tit++->second.c_str());
  it = result.find("WPP");
  EXPECT_NE(result.end(), it);
  EXPECT_EQ(5U, it->second.size());
  tit = it->second.begin();
  EXPECT_STREQ("Window", tit->first.c_str());
  EXPECT_STREQ("length=512, type=rectangular", tit++->second.c_str());
  EXPECT_STREQ("DWPT", tit->first.c_str());
  EXPECT_STREQ("", tit++->second.c_str());
  EXPECT_STREQ("SubbandEnergy", tit->first.c_str());
  EXPECT_STREQ("", tit++->second.c_str());
  EXPECT_STREQ("Log", tit->first.c_str());
  EXPECT_STREQ("", tit++->second.c_str());
  EXPECT_STREQ("DWPT", tit->first.c_str());
  EXPECT_STREQ("order=4, tree=1 2 3 3", tit++->second.c_str());
}

#include "tests/google/src/gtest_main.cc"
