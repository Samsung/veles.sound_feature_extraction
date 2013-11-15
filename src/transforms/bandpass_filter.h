/*! @file bandpass_filter.h
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

#ifndef SRC_TRANSFORMS_BANDPASS_FILTER_H_
#define SRC_TRANSFORMS_BANDPASS_FILTER_H_

#include "src/transforms/iir_filter_base.h"

namespace sound_feature_extraction {
namespace transforms {

/// @brief Discards the frequencies which are lower than the threshold.
class BandpassFilter : public IIRFilterBase,
                       public TransformLogger<BandpassFilter> {
 public:
  BandpassFilter() noexcept;

  TRANSFORM_INTRO("Bandpass", "Bandpass filter. Cuts off frequences higher "
                              "than \"frequency_high\" and lower than "
                              "\"frequency_low\".",
                  BandpassFilter)

  TP(frequency_high, int, kMaxFilterFrequency,
     "All frequencies higher will be cut.")
  TP(frequency_low, int, kMinFilterFrequency,
     "All frequencies lower will be cut.")

  virtual void Initialize() const override;

 protected:
  virtual std::shared_ptr<IIRFilter> CreateExecutor() const noexcept override;
  virtual void Execute(const std::shared_ptr<IIRFilter>& exec, const float* in,
                       float* out) const override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_BANDPASS_FILTER_H_
