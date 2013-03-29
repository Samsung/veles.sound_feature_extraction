/*! @file api.cc
 *  @brief Tests for libSpeechFeatureExtraction API.
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
#include <spfextr/api.h>

TEST(API, query_transforms_list) {
  char** names = nullptr;
  int count = 0;
  query_transforms_list(&names, &count);
  ASSERT_GT(count, 0);
  ASSERT_NE(nullptr, names);
  for (int i = 0; i < count; i++) {
    ASSERT_NE(nullptr, names[i]);
  }
  destroy_transforms_list(names, count);
}

TEST(API, query_transform_details) {
  char* description = nullptr;
  char *inputFormat;
  char *outputFormat;
  char** pnames = nullptr;
  char** pdescs = nullptr;
  char** pdefs = nullptr;
  int count = 0;
  query_transform_details("Window", &description, &inputFormat, &outputFormat,
                          &pnames, &pdescs, &pdefs, &count);
  ASSERT_NE(nullptr, description);
  ASSERT_NE(nullptr, pnames);
  ASSERT_NE(nullptr, pdescs);
  ASSERT_NE(nullptr, pdefs);
  ASSERT_GT(count, 0);
  for (int i = 0; i < count; i++) {
    ASSERT_NE(nullptr, pnames[i]);
    ASSERT_NE(nullptr, pdescs[i]);
    ASSERT_NE(nullptr, pdefs[i]);
  }
  destroy_transform_details(description, inputFormat, outputFormat,
                            pnames, pdescs, pdefs, count);
}

TEST(API, extract_speech_features) {

}

TEST(API, report_extraction_graph) {

}

#include "tests/google/src/gtest_main.cc"

