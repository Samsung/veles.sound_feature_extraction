/*! @file floatptr.h
 *  @brief Typedef of unique_ptr to aligned floating-point array and the
 *  thread-safe wrapper.
 *  @author Vadim Markovtsev <v.markovtsev@samsung.com>
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

#ifndef SRC_FLOATPTR_H_
#define SRC_FLOATPTR_H_

#include <cstdlib>
#include <mutex>

namespace sound_feature_extraction {

typedef std::unique_ptr<float[], decltype(&std::free)> FloatPtr;

struct ThreadSafeFloatPtr {
  ThreadSafeFloatPtr() : data(nullptr, std::free) {
  }

  ThreadSafeFloatPtr(const ThreadSafeFloatPtr&) : data(nullptr, std::free) {
  }

  FloatPtr data;
  std::mutex mutex;
};

}  // namespace sound_feature_extraction

#endif  // SRC_FLOATPTR_H_
