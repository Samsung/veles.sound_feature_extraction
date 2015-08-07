/*! @file lsp.h
 *  @brief Converts Linear Prediction Coefficients (LPC) to more robust
 *  Line Spectral Pairs (LSP).
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

#ifndef SRC_TRANSFORMS_LSP_H_
#define SRC_TRANSFORMS_LSP_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class LSP : public OmpUniformFormatTransform<formats::ArrayFormatF> {
 public:
  LSP();

  TRANSFORM_INTRO("LSP", "Converts Linear Prediction Coefficients (LPC) to "
                         "more robust Line Spectral Pairs (LSP) "
                         "representation.",
                  LSP)

  TP(intervals, int, kDefaultIntervals,
     "The number of intervals to scan in the range [-1, 1]. "
      "The corresponding delta equals to 2 / intervals. The "
      "bigger the number, the less probability is to skip a root.")
  TP(bisects, int, kDefaultBisects,
     "The number of bisections for the root value refinement. "
     "The bigger the number, the more precise the values are "
     "and the less probability is to skip a root.")

 protected:
  virtual void Do(const float* in, float* out) const noexcept override;

  static constexpr int kDefaultIntervals = 128;
  static constexpr int kDefaultBisects = 16;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_LSP_H_
