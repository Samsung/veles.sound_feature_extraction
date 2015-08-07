/*! @file shc.h
 *  @brief Prepare for fundamental frequency extraction using Spectral Harmonics Correlation.
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

#ifndef SRC_TRANSFORMS_SHC_H_
#define SRC_TRANSFORMS_SHC_H_

#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

class SHCInvalidMinMaxWindowWidthException : public ExceptionBase {
 public:
  explicit SHCInvalidMinMaxWindowWidthException(int min, int max, int window,
                                                int samplingRate)
  : ExceptionBase(std::string("SHC transform was set min=") +
                  std::to_string(min) + ", max=" + std::to_string(max) +
                  " while window width was set to " + std::to_string(window) +
                  " and sampling rate is " + std::to_string(samplingRate) +
                  ".") {
  }
};

/// @brief Prepare for fundamental frequency extraction using Spectral Harmonics
/// Correlation.
class SHC : public OmpUniformFormatTransform<formats::ArrayFormatF>,
      public TransformLogger<SHC> {
 public:
  SHC();

  TRANSFORM_INTRO("SHC", "Apply spectral harmonics correlation.", SHC)

  TP(harmonics, int, kDefaultHarmonicsNumber, "The number of harmonics.")
  TP(window, int, kDefaultWindowWidth, "The spectral window length, in Hz.")
  TP(min, int, kDefaultMinFrequency, "The minimal frequency to scan.")
  TP(max, int, kDefaultMaxFrequency, "The maximal frequency to scan.")

 protected:
  virtual void Initialize() const override;
  virtual size_t OnFormatChanged(size_t buffersCount) override;
  virtual void Do(const float* in, float* out) const noexcept override;

 private:
  static constexpr int kDefaultHarmonicsNumber = 3;
  static constexpr int kDefaultWindowWidth = 40;
  static constexpr int kDefaultMinFrequency = 50;
  static constexpr int kDefaultMaxFrequency = 600;

  mutable int half_window_samples_;
  mutable int min_samples_;
  mutable int max_samples_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_SHC_H_
