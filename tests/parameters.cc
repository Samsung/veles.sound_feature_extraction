/*! @file parameters.cc
 *  @brief Tests for src/parameters.h.
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
#include "src/parameters.h"

using SpeechFeatureExtraction::Parameters::Parse;
using SpeechFeatureExtraction::Parameters::ParseParametersException;

TEST(Parameters, EmptyParse) {
  auto pp = Parse("");
  ASSERT_EQ(0, pp.size());
}

TEST(Parameters, Parse) {
  auto pp = Parse("one=56, two = some extra , \t three with spaces= xxx ");
  ASSERT_EQ(3, pp.size());
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
  bool fail = false;
  try {
    auto pp = Parse("256");
  }
  catch(ParseParametersException* e) {
    fail = true;
  }
  ASSERT_TRUE(fail);
}

#include "tests/google/src/gtest_main.cc"
