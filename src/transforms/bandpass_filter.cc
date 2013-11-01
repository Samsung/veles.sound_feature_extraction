/*! @file bandpass_filter.cc
 *  @brief Bandpass filter.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/bandpass_filter.h"
#include <math.h>

namespace sound_feature_extraction {
namespace transforms {

constexpr const char* BandpassFilter::kHighFrequencyParameterName;
constexpr const char* BandpassFilter::kLowFrequencyParameterName;

BandpassFilter::BandpassFilter() noexcept
    : frequency_high_(kMaxFilterFrequency),
      frequency_low_(kMinFilterFrequency) {
  RegisterSetter(kHighFrequencyParameterName, [&](const std::string& value) {
    int pv = Parse<int>(kHighFrequencyParameterName, value);
    if (pv < kMinFilterFrequency || pv > kMaxFilterFrequency) {
      return false;
    }
    frequency_high_ = pv;
    return true;
  });
  RegisterSetter(kLowFrequencyParameterName, [&](const std::string& value) {
    int pv = Parse<int>(kLowFrequencyParameterName, value);
    if (pv < kMinFilterFrequency || pv > kMaxFilterFrequency) {
      return false;
    }
    frequency_low_ = pv;
    return true;
  });
}


void BandpassFilter::Initialize() const {
  if (frequency_high_ <= frequency_low_) {
    ERR("Invalid frequencies. High is %d, low is %i.", frequency_high_,
        frequency_low_);
  }
  IIRFilterBase::Initialize();
}

int BandpassFilter::frequency_high() const {
  return frequency_high_;
}

int BandpassFilter::frequency_low() const {
  return frequency_low_;
}

void BandpassFilter::set_frequency_high(int value) {
  SetParameter(kHighFrequencyParameterName, std::to_string(value));
}

void BandpassFilter::set_frequency_low(int value) {
  SetParameter(kLowFrequencyParameterName, std::to_string(value));
}

std::shared_ptr<IIRFilter> BandpassFilter::CreateExecutor() const noexcept {
  auto center_freq = (frequency_high_ + frequency_low_) / 2;
  auto width_freq = frequency_high_ - frequency_low_;
  switch (type()) {
    case kIIRFilterTypeBessel: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Bessel::BandPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), inputFormat_->SamplingRate(), center_freq,
                 width_freq);
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case kIIRFilterTypeButterworth: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Butterworth::BandPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), inputFormat_->SamplingRate(), center_freq,
                 width_freq);
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case kIIRFilterTypeChebyshevI: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::ChebyshevI::BandPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), inputFormat_->SamplingRate(), center_freq,
                 width_freq, ripple());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case kIIRFilterTypeChebyshevII: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::ChebyshevII::BandPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), inputFormat_->SamplingRate(), center_freq,
                 width_freq, ripple());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case kIIRFilterTypeElliptic: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Elliptic::BandPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), inputFormat_->SamplingRate(), center_freq,
                 width_freq, ripple(), rolloff());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case kIIRFilterTypeLegendre: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Legendre::BandPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), inputFormat_->SamplingRate(), center_freq,
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
    case kIIRFilterTypeBessel:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::Bessel::BandPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case kIIRFilterTypeButterworth:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::Butterworth::BandPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case kIIRFilterTypeChebyshevI:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::ChebyshevI::BandPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case kIIRFilterTypeChebyshevII:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::ChebyshevII::BandPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case kIIRFilterTypeElliptic:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::Elliptic::BandPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case kIIRFilterTypeLegendre:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter< Dsp::Legendre::BandPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    default:
      break;
  }
}

REGISTER_TRANSFORM(BandpassFilter);

}  // namespace transforms
}  // namespace sound_feature_extraction
