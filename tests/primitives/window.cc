/*! @file window.cc
 *  @brief Tests for src/primitives/window.h.
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
#include <vector>
#include "src/primitives/window.h"

#define EPSILON 0.0001f

#define ASSERT_EQF(a, b) do { \
  ASSERT_GT(a + EPSILON, b); \
  ASSERT_LT(a - EPSILON, b); \
} while (0)

TEST(Window, Rectangular) {
  ASSERT_EQ(1.0f, WindowElement(WINDOW_TYPE_RECTANGULAR, 4, 0));
  ASSERT_EQ(1.0f, WindowElement(WINDOW_TYPE_RECTANGULAR, 5, 2));
  ASSERT_EQ(1.0f, WindowElement(WINDOW_TYPE_RECTANGULAR, 10, 8));
}

TEST(Window, Hamming) {
  std::vector<float> filter(20);
  std::vector<float> valid { 0.0800f, 0.7700f, 0.7700f, 0.0800f };  // NOLINT(*)
  for (size_t i = 0; i < valid.size(); i++) {
    filter[i] = WindowElement(WINDOW_TYPE_HAMMING, 4, i);
  }
  for (size_t i = 0; i < valid.size(); i++) {
    ASSERT_EQF(valid[i], filter[i]);
  }

  valid = { 0.0800f, 0.5400f, 1.0000f, 0.5400f, 0.0800f };
  for (size_t i = 0; i < valid.size(); i++) {
    filter[i] = WindowElement(WINDOW_TYPE_HAMMING, 5, i);
  }
  for (size_t i = 0; i < valid.size(); i++) {
    ASSERT_EQF(valid[i], filter[i]);
  }

  valid = { 0.0800f, 0.1077f, 0.1876f, 0.3100f, 0.4601f,
            0.6199f, 0.7700f, 0.8924f, 0.9723f, 1.0000f,
            0.9723f, 0.8924f, 0.7700f, 0.6199f, 0.4601f,
            0.3100f, 0.1876f, 0.1077f, 0.0800f };
  for (size_t i = 0; i < valid.size(); i++) {
    filter[i] = WindowElement(WINDOW_TYPE_HAMMING, 19, i);
  }
  for (size_t i = 0; i < valid.size(); i++) {
    ASSERT_EQF(valid[i], filter[i]);
  }

  valid = { 0.0800f, 0.1049f, 0.1770f, 0.2884f, 0.4271f,
            0.5780f, 0.7248f, 0.8515f, 0.9446f, 0.9937f,
            0.9937f, 0.9446f, 0.8515f, 0.7248f, 0.5780f,
            0.4271f, 0.2884f, 0.1770f, 0.1049f, 0.0800f };
  for (size_t i = 0; i < valid.size(); i++) {
    filter[i] = WindowElement(WINDOW_TYPE_HAMMING, 20, i);
  }
  for (size_t i = 0; i < valid.size(); i++) {
    ASSERT_EQF(valid[i], filter[i]);
  }
}

#include "tests/google/src/gtest_main.cc"
