/*! @file bandpass_filter.cc
 *  @brief Bandpass filter.
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

#include "src/transforms/bandpass_filter.h"
#include <cmath>

namespace sound_feature_extraction {
namespace transforms {

BandpassFilter::BandpassFilter() noexcept
    : frequency_high_(kMaxFilterFrequency),
      frequency_low_(kMinFilterFrequency) {
}


bool BandpassFilter::validate_frequency_high(const int& value) noexcept {
  return ValidateFrequency(value);
}

bool BandpassFilter::validate_frequency_low(const int& value) noexcept {
  return ValidateFrequency(value);
}

void BandpassFilter::Initialize() const {
  if (frequency_high_ <= frequency_low_) {
    ERR("Invalid frequencies. High is %d, low is %i.", frequency_high_,
        frequency_low_);
  }
  IIRFilterBase::Initialize();
}

std::shared_ptr<IIRFilter> BandpassFilter::CreateExecutor() const noexcept {
  auto center_freq = (frequency_high_ + frequency_low_) / 2;
  auto width_freq = frequency_high_ - frequency_low_;
  switch (type()) {
    case IIRFilterType::kBessel: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Bessel::BandPass<kMaxFilterLength>,
          1, Dsp::DirectFormI<float>>>();
      ret->setup(length(), input_format_->SamplingRate(), center_freq,
                 width_freq);
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kButterworth: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Butterworth::BandPass<kMaxFilterLength>,
          1, Dsp::DirectFormI<float>>>();
      ret->setup(length(), input_format_->SamplingRate(), center_freq,
                 width_freq);
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kChebyshevI: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::ChebyshevI::BandPass<kMaxFilterLength>,
          1, Dsp::DirectFormI<float>>>();
      ret->setup(length(), input_format_->SamplingRate(), center_freq,
                 width_freq, ripple());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kChebyshevII: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::ChebyshevII::BandPass<kMaxFilterLength>,
          1, Dsp::DirectFormI<float>>>();
      ret->setup(length(), input_format_->SamplingRate(), center_freq,
                 width_freq, ripple());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kElliptic: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Elliptic::BandPass<kMaxFilterLength>,
          1, Dsp::DirectFormI<float>>>();
      ret->setup(length(), input_format_->SamplingRate(), center_freq,
                 width_freq, ripple(), rolloff());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kLegendre: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Legendre::BandPass<kMaxFilterLength>,
          1, Dsp::DirectFormI<float>>>();
      ret->setup(length(), input_format_->SamplingRate(), center_freq,
                 width_freq);
      return std::static_pointer_cast<IIRFilter>(ret);
    }
  }
  return nullptr;
}

void BandpassFilter::Execute(const std::shared_ptr<IIRFilter>& exec,
                            const float* in,
                            float* out) const {
  std::shared_ptr<IIRFilter> ptr = exec;
  switch (type()) {
    case IIRFilterType::kBessel:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<
                  Dsp::Bessel::BandPass<kMaxFilterLength>,
                  1, Dsp::DirectFormI<float>>
          >(ptr),
          in, out);
      break;
    case IIRFilterType::kButterworth:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<
                  Dsp::Butterworth::BandPass<kMaxFilterLength>,
                  1, Dsp::DirectFormI<float>>
          >(ptr),
          in, out);
      break;
    case IIRFilterType::kChebyshevI:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<
                  Dsp::ChebyshevI::BandPass<kMaxFilterLength>,
                  1, Dsp::DirectFormI<float>>
          >(ptr),
          in, out);
      break;
    case IIRFilterType::kChebyshevII:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<
                  Dsp::ChebyshevII::BandPass<kMaxFilterLength>,
                  1, Dsp::DirectFormI<float>>>(ptr),
          in, out);
      break;
    case IIRFilterType::kElliptic:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<
                  Dsp::Elliptic::BandPass<kMaxFilterLength>,
                  1, Dsp::DirectFormI<float>>
          >(ptr),
          in, out);
      break;
    case IIRFilterType::kLegendre:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<
                  Dsp::Legendre::BandPass<kMaxFilterLength>,
                  1, Dsp::DirectFormI<float>>
          >(ptr),
          in, out);
      break;
    default:
      break;
  }
}

RTP(BandpassFilter, frequency_low)
RTP(BandpassFilter, frequency_high)
REGISTER_TRANSFORM(BandpassFilter);

}  // namespace transforms
}  // namespace sound_feature_extraction
