/*! @file simd_aware.h
 *  @brief All classes which support SIMD operations should inherit from this.
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

#ifndef SRC_SIMD_AWARE_H_
#define SRC_SIMD_AWARE_H_

void set_use_simd(int /* value */);

namespace sound_feature_extraction {

class SimdAware {
  friend void ::set_use_simd(int /* value */);
 public:
  static bool use_simd() noexcept {
    return use_simd_;
  }

 protected:
  static void set_use_simd(bool value) noexcept {
    use_simd_ = value;
  }

  static bool use_simd_;
};

}  // namespace sound_feature_extraction

#endif  // SRC_SIMD_AWARE_H_
