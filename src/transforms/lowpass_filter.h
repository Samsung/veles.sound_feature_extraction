/*! @file lowpass_filter.h
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

#ifndef SRC_TRANSFORMS_LOWPASS_FILTER_H_
#define SRC_TRANSFORMS_LOWPASS_FILTER_H_

#include "src/transforms/iir_filter_base.h"
#include "src/transforms/single_frequency_filter.h"

namespace sound_feature_extraction {
namespace transforms {

/// @brief Discards the frequencies which are higher than the threshold.
class LowpassFilter : public SingleFrequencyFilter<IIRFilterBase> {
 public:
  TRANSFORM_INTRO("Lowpass", "Lowpass filter. All frequences higher than "
                             "\"frequency\" shall not pass.")

  IIR_FILTER_PARAMETERS(
      TP("frequency", "Suppress frequencies higher than this value.",
         std::to_string(kMinFilterFrequency))
  )

 protected:
  virtual std::shared_ptr<IIRFilter> CreateExecutor() const noexcept override;
  virtual void Execute(const std::shared_ptr<IIRFilter>& exec, const float* in,
                       float* out) const override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_LOWPASS_FILTER_H_
