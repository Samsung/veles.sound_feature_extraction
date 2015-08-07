/*! @file window_splitter.cc
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

#include "src/transforms/window_splitter.h"
#include <simd/arithmetic-inl.h>

namespace sound_feature_extraction {
namespace transforms {

void WindowSplitter16::Do(const BuffersBase<int16_t*>& in,
                          BuffersBase<int16_t*> *out)
const noexcept {
#ifdef __AVX__
  int16_t intbuf[output_format_->Size()] __attribute__ ((aligned (32)));  // NOLINT(*)
#endif
  float fbuf[output_format_->Size()] __attribute__ ((aligned (64)));  // NOLINT(*)
  float* window = window_.get();

  for (size_t i = 0; i < in.Count(); i++) {
    for (int j = 0; j < windows_count_; j++) {
      auto input = in[i] + j * step();
      auto output = interleaved()? (*out)[i * windows_count_ + j] :
                                  (*out)[j * in.Count() + i];
      if (type() != WindowType::kWindowTypeRectangular) {
#ifdef __AVX__
        if (align_complement_i16(input) != 0) {
          memcpy(intbuf, input, output_format_->Size() * sizeof(int16_t));
          int16_to_float(intbuf, output_format_->Size(), fbuf);
        } else {
          int16_to_float(input, output_format_->Size(), fbuf);
        }
#else
        int16_to_float(input, output_format_->Size(), fbuf);
#endif
        Window::ApplyWindow(use_simd(), window, output_format_->Size(),
                            fbuf, fbuf);
        float_to_int16(fbuf, output_format_->Size(), output);
      } else {  // type() != kWindowTypeRectangular
        memcpy(output, input, output_format_->Size() * sizeof(input[0]));
      }
    }
  }
}

void WindowSplitterF::Do(const BuffersBase<float*>& in,
                         BuffersBase<float*> *out)
const noexcept {
#ifdef __AVX__
  float intbuf[output_format_->Size()] __attribute__ ((aligned (32)));  // NOLINT(*)
#endif
  for (size_t i = 0; i < in.Count(); i++) {
    for (int j = 0; j < windows_count_; j++) {
      auto input = in[i] + j * step();
      auto output = (*out)[i * windows_count_ + j];
      if (type() != WindowType::kWindowTypeRectangular) {
#ifdef __AVX__
        if (align_complement_f32(input) != 0) {
          memcpy(intbuf, input, output_format_->Size() * sizeof(input[0]));
          Window::ApplyWindow(use_simd(), window_.get(), output_format_->Size(),
                              intbuf, output);
        } else {
#endif
        Window::ApplyWindow(use_simd(), window_.get(), output_format_->Size(),
                            input, output);
#ifdef __AVX__
        }
#endif
      } else {
        memcpy(output, input, output_format_->Size() * sizeof(input[0]));
      }
    }
  }
}

REGISTER_TRANSFORM(WindowSplitter16);
REGISTER_TRANSFORM(WindowSplitterF);
REGISTER_TRANSFORM(WindowSplitter16Inverse);
REGISTER_TRANSFORM(WindowSplitterFInverse);

}  // namespace transforms
}  // namespace sound_feature_extraction
