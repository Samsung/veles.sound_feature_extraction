/*! @file transform_test.h
 *  @brief New file description.
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
        this->input_format_, buffersCount);
    RecreateOutputBuffers();
    this->Initialize();
  }

  void RecreateOutputBuffers() {
    size_t count = this->SetInputFormat(this->input_format_, Input->Count());
    Output = std::static_pointer_cast<typename T::OutBuffers>(
        this->CreateOutputBuffers(count));
  }
};

#include "tests/google/src/gtest_main.cc"

#endif  // TESTS_TRANSFORMS_TRANSFORM_TEST_H_
