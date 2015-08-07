/*! @file convolve.h
 *  @brief Convolve a raw signal with a window function.
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

#ifndef SRC_TRANSFORMS_CONVOLVE_H_
#define SRC_TRANSFORMS_CONVOLVE_H_

#include "src/transforms/fir_filter_base.h"
#include "src/primitives/window.h"

namespace sound_feature_extraction {
namespace transforms {

/// @brief Convolves a raw signal with a window function.
class ConvolveFilter : public FIRFilterBase {
 public:
  ConvolveFilter() noexcept;

  TRANSFORM_INTRO("Convolve", "Convolve a raw signal with a window function.",
                  ConvolveFilter)

  TP(window, WindowType, kDefaultWindowType,
     "Type of the window. E.g. \"rectangular\" or \"hamming\".")

 protected:
  static constexpr WindowType kDefaultWindowType =
      WindowType::kWindowTypeRectangular;
  virtual void CalculateFilter(float* filter) const noexcept;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_CONVOLVE_H_
