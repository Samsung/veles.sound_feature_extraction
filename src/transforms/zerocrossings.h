/*! @file zerocrossings.h
 *  @brief Number of time domain zero crossings of the signal.
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

#ifndef SRC_TRANSFORMS_ZEROCROSSINGS_H_
#define SRC_TRANSFORMS_ZEROCROSSINGS_H_

#include "src/formats/array_format.h"
#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

template <class F>
using ZeroCrossingsTemplateBase = OmpTransformBase<F, formats::SingleFormat32>;

template <class F>
class ZeroCrossingsTemplate : public ZeroCrossingsTemplateBase<F> {
 public:
  TRANSFORM_INTRO("ZeroCrossings", "Number of time domain zero crossings "
                                   "of the signal.",
                  ZeroCrossingsTemplateBase<F>)

 protected:
  virtual void Do(const typename F::BufferElementType* in,
                  int32_t* out) const noexcept override final {
    auto result = DoInternal(this->use_simd(), in,
                             this->input_format_->Size());
    assert(result >= 0 &&
           result <= static_cast<int>(this->input_format_->Size()));
    *out = result;
  }

  virtual int DoInternal(bool simd, const typename F::BufferElementType* input,
                         size_t length) const noexcept = 0;
};

class ZeroCrossingsF
    : public ZeroCrossingsTemplate<formats::ArrayFormatF> {
 protected:
  virtual int DoInternal(bool simd, const float* input, size_t length)
      const noexcept override;
};

class ZeroCrossings16
    : public ZeroCrossingsTemplate<formats::ArrayFormat16> {
 protected:
  virtual int DoInternal(bool simd, const int16_t* input, size_t length)
      const noexcept override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_ZEROCROSSINGS_H_
