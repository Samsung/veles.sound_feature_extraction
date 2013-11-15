/*! @file highpass_filter.h
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

#ifndef SRC_TRANSFORMS_HIGHPASS_FILTER_H_
#define SRC_TRANSFORMS_HIGHPASS_FILTER_H_

#include "src/transforms/iir_filter_base.h"
#include "src/transforms/single_frequency_filter.h"

namespace sound_feature_extraction {
namespace transforms {

using HighpassFilterBase = SingleFrequencyFilter<IIRFilterBase>;

/// @brief Discards the frequencies which are lower than the threshold.
class HighpassFilter : public HighpassFilterBase {
 public:
  TRANSFORM_INTRO("Highpass", "Highpass filter. All frequences lower than "
                              "\"frequency\" shall not pass.",
                  HighpassFilter)

 protected:
  virtual std::shared_ptr<IIRFilter> CreateExecutor() const noexcept override;
  virtual void Execute(const std::shared_ptr<IIRFilter>& exec, const float* in,
                       float* out) const override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_HIGHPASS_FILTER_H_
