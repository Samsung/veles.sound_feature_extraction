/*! @file lowpass_filter.cc
 *  @brief Lowpass filter.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/lowpass_filter.h"
#include <cmath>

namespace sound_feature_extraction {
namespace transforms {

std::shared_ptr<IIRFilter> LowpassFilter::CreateExecutor() const noexcept {
  switch (type()) {
    case IIRFilterType::kBessel: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Bessel::LowPass<kMaxFilterLength>,
          1, Dsp::DirectFormI<float>>>();
      ret->setup(length(), input_format_->SamplingRate(), frequency());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kButterworth: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Butterworth::LowPass<kMaxFilterLength>,
          1, Dsp::DirectFormI<float>>>();
      ret->setup(length(), input_format_->SamplingRate(), frequency());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kChebyshevI: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::ChebyshevI::LowPass<kMaxFilterLength>,
          1, Dsp::DirectFormI<float>>>();
      ret->setup(length(), input_format_->SamplingRate(), frequency(),
                 ripple());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kChebyshevII: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::ChebyshevII::LowPass<kMaxFilterLength>,
          1, Dsp::DirectFormI<float>>>();
      ret->setup(length(), input_format_->SamplingRate(), frequency(),
                 ripple());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kElliptic: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Elliptic::LowPass<kMaxFilterLength>,
          1, Dsp::DirectFormI<float>>>();
      ret->setup(length(), input_format_->SamplingRate(), frequency(),
                 ripple(), rolloff());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case IIRFilterType::kLegendre: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Legendre::LowPass<kMaxFilterLength>,
          1, Dsp::DirectFormI<float>>>();
      ret->setup(length(), input_format_->SamplingRate(), frequency());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
  }
  return nullptr;
}

void LowpassFilter::Execute(const std::shared_ptr<IIRFilter>& exec,
                            const float* in,
                            float* out) const {
  std::shared_ptr<IIRFilter> ptr = exec;
  switch (type()) {
    case IIRFilterType::kBessel:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<
                  Dsp::Bessel::LowPass<kMaxFilterLength>,
                  1, Dsp::DirectFormI<float>>
          >(ptr),
          in, out);
      break;
    case IIRFilterType::kButterworth:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<
                  Dsp::Butterworth::LowPass<kMaxFilterLength>,
                  1, Dsp::DirectFormI<float>>
          >(ptr),
          in, out);
      break;
    case IIRFilterType::kChebyshevI:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<
                  Dsp::ChebyshevI::LowPass<kMaxFilterLength>,
                  1, Dsp::DirectFormI<float>>
          >(ptr),
          in, out);
      break;
    case IIRFilterType::kChebyshevII:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<
                  Dsp::ChebyshevII::LowPass<kMaxFilterLength>,
                  1, Dsp::DirectFormI<float>>>(ptr),
          in, out);
      break;
    case IIRFilterType::kElliptic:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<
                  Dsp::Elliptic::LowPass<kMaxFilterLength>,
                  1, Dsp::DirectFormI<float>>
          >(ptr),
          in, out);
      break;
    case IIRFilterType::kLegendre:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<
                  Dsp::Legendre::LowPass<kMaxFilterLength>,
                  1, Dsp::DirectFormI<float>>
          >(ptr),
          in, out);
      break;
    default:
      break;
  }
}

REGISTER_TRANSFORM(LowpassFilter);

}  // namespace transforms
}  // namespace sound_feature_extraction
