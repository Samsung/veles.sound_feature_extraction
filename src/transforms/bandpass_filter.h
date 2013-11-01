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
                              "\"frequency_low\".")

  IIR_FILTER_PARAMETERS(
      TP(kHighFrequencyParameterName, "All frequencies higher will be cut",
         std::to_string(kMaxFilterFrequency))
      TP(kLowFrequencyParameterName, "All frequencies lower will be cut",
         std::to_string(kMinFilterFrequency))
  )

  int frequency_high() const;
  int frequency_low() const;
  void set_frequency_high(int value);
  void set_frequency_low(int value);

  virtual void Initialize() const override;

  static constexpr const char* kHighFrequencyParameterName = "frequency_high";
  static constexpr const char* kLowFrequencyParameterName = "frequency_low";

 protected:
  virtual std::shared_ptr<IIRFilter> CreateExecutor() const noexcept override;
  virtual void Execute(const std::shared_ptr<IIRFilter>& exec, const float* in,
                       float* out) const override;

 private:
  int frequency_high_;
  int frequency_low_;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_BANDPASS_FILTER_H_
