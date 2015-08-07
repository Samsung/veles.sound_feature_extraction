/*! @file mix_stereo.cc
 *  @brief Mix audio channels together.
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

#include "src/transforms/mix_stereo.h"
#include <simd/instruction_set.h>

namespace sound_feature_extraction {
namespace transforms {

size_t MixStereo::OnFormatChanged(size_t buffersCount) {
  output_format_->SetSize(input_format_->Size() / 2);
  return buffersCount;
}

void MixStereo::Do(const int16_t* in, int16_t* out) const noexcept {
  int length = input_format_->Size();
  if (use_simd()) {
#ifdef __AVX__
    int offset = align_complement_i16(in);
    for (int i = 0; i < offset; i += 2) {
      int16_t l = in[i] / 2;
      int16_t r = in[i + 1] / 2;
      out[i / 2] = l + r;
    }
    for (int i = offset; i < length - offset - 15; i += 16) {
      __m128i vec1 = _mm_load_si128(
          reinterpret_cast<const __m128i*>(in + i));
      __m128i vec2 = _mm_load_si128(
          reinterpret_cast<const __m128i*>(in + i + 8));
      vec1 = _mm_srai_epi16(vec1, 1);
      vec2 = _mm_srai_epi16(vec2, 1);
      __m128i res = _mm_hadd_epi16(vec1, vec2);
      _mm_storeu_si128(reinterpret_cast<__m128i*>(out + i / 2), res);
    }
    for (int i = ((length - offset) & ~0xF); i < length; i += 2) {
      int16_t l = in[i] / 2;
      int16_t r = in[i + 1] / 2;
      out[i / 2] = l + r;
    }
  } else {
#elif defined(__ARM_NEON__)
    for (int i = 0; i < length - 15; i += 16) {
      int16x8x2_t vec = vld2q_s16(in + i);
      int16x8_t res = vshrq_n_s16(vec.val[0], 1);
      res = vsraq_n_s16(res, vec.val[1], 1);
      vst1q_s16(out + i / 2, res);
    }
    for (int i = (length & ~0xF); i < length; i += 2) {
      int16_t l = in[i] / 2;
      int16_t r = in[i + 1] / 2;
      out[i / 2] = l + r;
    }
  } else {
#else
  } {
#endif
    for (int i = 0; i < length; i += 2) {
      int16_t l = in[i] / 2;
      int16_t r = in[i + 1] / 2;
      out[i / 2] = l + r;
    }
  }
}


REGISTER_TRANSFORM(MixStereo);

}  // namespace transforms
}  // namespace sound_feature_extraction
