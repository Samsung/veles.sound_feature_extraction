/*! @file highpass_filter.cc
 *  @brief Highpass filter.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/highpass_filter.h"
#include <cmath>

namespace sound_feature_extraction {
namespace transforms {

std::shared_ptr<IIRFilter> HighpassFilter::CreateExecutor() const noexcept {
  switch (type()) {
    case IIRFilterType::kBessel: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Bessel::HighPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), input_format_->SamplingRate(), frequency());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kButterworth: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Butterworth::HighPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), input_format_->SamplingRate(), frequency());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kChebyshevI: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::ChebyshevI::HighPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), input_format_->SamplingRate(), frequency(),
                 ripple());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kChebyshevII: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::ChebyshevII::HighPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), input_format_->SamplingRate(), frequency(),
                 ripple());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kElliptic: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Elliptic::HighPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), input_format_->SamplingRate(), frequency(),
                 ripple(), rolloff());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kLegendre: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Legendre::HighPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), input_format_->SamplingRate(), frequency());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
  }
  return nullptr;
}

void HighpassFilter::Execute(const std::shared_ptr<IIRFilter>& exec,
                             const float* in,
                             float* out) const {
  std::shared_ptr<IIRFilter> ptr = exec;
  switch (type()) {
    case IIRFilterType::kBessel:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::Bessel::HighPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case IIRFilterType::kButterworth:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::Butterworth::HighPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case IIRFilterType::kChebyshevI:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::ChebyshevI::HighPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case IIRFilterType::kChebyshevII:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::ChebyshevII::HighPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case IIRFilterType::kElliptic:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::Elliptic::HighPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case IIRFilterType::kLegendre:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter< Dsp::Legendre::HighPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    default:
      break;
  }
}

REGISTER_TRANSFORM(HighpassFilter);

}  // namespace transforms
}  // namespace sound_feature_extraction
