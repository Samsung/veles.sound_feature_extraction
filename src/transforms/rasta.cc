/*! @file rasta.cc
 *  @brief RASTA IIR filtering transform implementation.
 *  recursion.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2015 Samsung R&D Institute Russia
 */

#include "src/transforms/rasta.h"

namespace sound_feature_extraction {
namespace transforms {

RASTA::RASTA() noexcept
    : pole_(kDefaultPole){
}


bool RASTA::validate_pole(const float& value) noexcept {
  return value < 1 && value > 0;
}

std::shared_ptr<IIRFilter> RASTA::CreateExecutor() const noexcept {
  auto ret = std::make_shared<Dsp::SimpleFilter<
      Dsp::RASTA::Filter, 1, Dsp::DirectFormI<float>>>();
  ret->setup(pole());
  return std::static_pointer_cast<IIRFilter>(ret);
}

void RASTA::Execute(const std::shared_ptr<IIRFilter>& exec,
                    const float* in, float* out) const {
  std::shared_ptr<IIRFilter> ptr = exec;
  IIRFilterBase::Execute(
      std::static_pointer_cast<
          Dsp::SimpleFilter<Dsp::RASTA::Filter, 1, Dsp::DirectFormI<float>>
      >(ptr), in, out);
}

RTP(RASTA, pole)
REGISTER_TRANSFORM(RASTA);

}  // namespace transforms
}  // namespace sound_feature_extraction
