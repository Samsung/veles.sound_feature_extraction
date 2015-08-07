/*! @file simd_aware.cc
 *  @brief sound_feature_extraction::SimdAware::useSimd_ initialization.
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


#include "src/simd_aware.h"
#ifdef VALGRIND
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <valgrind.h>
#pragma GCC diagnostic pop
#else
#define RUNNING_ON_VALGRIND false
#endif

namespace sound_feature_extraction {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
static bool under_valgrind() noexcept {
  return RUNNING_ON_VALGRIND;
}
#pragma GCC diagnostic pop

bool SimdAware::use_simd_ = !under_valgrind();

}  // namespace sound_feature_extraction

