/*! @file window.h
 *  @brief Windowing transform.
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

#ifndef SRC_TRANSFORMS_WINDOW_H_
#define SRC_TRANSFORMS_WINDOW_H_

#include "src/transforms/common.h"
#include "src/primitives/window.h"

namespace sound_feature_extraction {
namespace transforms {

//// @brief Applies a window function to each buffer.
class Window : public OmpUniformFormatTransform<formats::ArrayFormatF> {
  template <class T> friend class WindowSplitterTemplate;
  friend class WindowSplitter16;
  friend class WindowSplitterF;
 public:
  Window();

  TRANSFORM_INTRO("WindowFunction",
                  "Applies a window function to each window.",
                  Window)

  TP(type, WindowType, kDefaultType,
     "Type of the window. E.g. \"rectangular\" or \"hamming\".")
  TP(predft, bool, kDefaultPreDft,
     "Apply Discrete Fourier Transform to window function.")

  virtual void Initialize() const override;

 protected:
  typedef std::unique_ptr<float, void(*)(void*)> WindowContentsPtr;
  static constexpr WindowType kDefaultType = WindowType::kWindowTypeHamming;
  static constexpr bool kDefaultPreDft = false;

  virtual void Do(const float* in,
                  float* out) const noexcept override;

  mutable WindowContentsPtr window_;

  static void ApplyWindow(bool simd, const float* window, int length,
                          const float* input, float* output) noexcept;

 private:
  static WindowContentsPtr InitializeWindow(size_t length,
                                            WindowType type,
                                            int allocSize = -1) noexcept;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_WINDOW_H_
