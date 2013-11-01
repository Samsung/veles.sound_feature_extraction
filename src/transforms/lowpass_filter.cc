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
#include <math.h>

namespace sound_feature_extraction {
namespace transforms {

std::shared_ptr<IIRFilter> LowpassFilter::CreateExecutor() const noexcept {
  switch (type()) {
    case kIIRFilterTypeBessel: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Bessel::LowPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), inputFormat_->SamplingRate(), frequency());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case kIIRFilterTypeButterworth: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Butterworth::LowPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), inputFormat_->SamplingRate(), frequency());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case kIIRFilterTypeChebyshevI: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::ChebyshevI::LowPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), inputFormat_->SamplingRate(), frequency(), ripple());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case kIIRFilterTypeChebyshevII: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::ChebyshevII::LowPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), inputFormat_->SamplingRate(), frequency(), ripple());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case kIIRFilterTypeElliptic: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Elliptic::LowPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), inputFormat_->SamplingRate(), frequency(), ripple(),
                 rolloff());
      return std::static_pointer_cast<IIRFilter>(ret);
    }
    case kIIRFilterTypeLegendre: {
      auto ret = std::make_shared<Dsp::SimpleFilter<
          Dsp::Legendre::LowPass<kMaxFilterLength>, 1>>();
      ret->setup(length(), inputFormat_->SamplingRate(), frequency());
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
    case kIIRFilterTypeBessel:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::Bessel::LowPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case kIIRFilterTypeButterworth:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::Butterworth::LowPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case kIIRFilterTypeChebyshevI:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::ChebyshevI::LowPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case kIIRFilterTypeChebyshevII:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::ChebyshevII::LowPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case kIIRFilterTypeElliptic:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter<Dsp::Elliptic::LowPass<kMaxFilterLength>, 1>
          >(ptr),
          in, out);
      break;
    case kIIRFilterTypeLegendre:
      IIRFilterBase::Execute(
          std::static_pointer_cast<
              Dsp::SimpleFilter< Dsp::Legendre::LowPass<kMaxFilterLength>, 1>
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
