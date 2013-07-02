/*! @file transform_test.h
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef TESTS_TRANSFORMS_TRANSFORM_TEST_H_
#define TESTS_TRANSFORMS_TRANSFORM_TEST_H_

#include <gtest/gtest.h>
#include <memory>

template <class T>
class TransformTest
    : public T,
      public testing::Test {
 public:
  std::shared_ptr<typename T::InBuffers> Input;
  std::shared_ptr<typename T::OutBuffers> Output;

  template <typename... Args>
  void SetUpTransform(size_t buffersCount, Args... args) {
    this->SetInputFormat(std::make_shared<typename T::InFormat>(args...),
                         buffersCount);
    Input = std::make_shared<typename T::InBuffers>(
        this->inputFormat_, buffersCount);
    RecreateOutputBuffers();
    this->Initialize();
  }

  void RecreateOutputBuffers() {
    size_t count = this->SetInputFormat(this->inputFormat_, Input->Count());
    Output = std::static_pointer_cast<typename T::OutBuffers>(
        this->CreateOutputBuffers(count));
  }
};

#include "tests/google/src/gtest_main.cc"

#endif  // TESTS_TRANSFORMS_TRANSFORM_TEST_H_
