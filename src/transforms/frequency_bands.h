/*! @file frequency_bands.h
 *  @brief Cut the frequency bands from a signal.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_TEMPLATE_TRANSFORM_H_
#define SRC_TRANSFORMS_TEMPLATE_TRANSFORM_H_

#include <vector>
#include "src/transforms/iir_filter_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class FrequencyBands
    : public UniformFormatOmpAwareTransform<Formats::ArrayFormatF>,
      public TransformLogger<FrequencyBands> {
 public:
  FrequencyBands();

  TRANSFORM_INTRO("FrequencyBands",
                  "Cut the frequency bands from sequential windows.")

  TRANSFORM_PARAMETERS(
      TP("number", "The number of bands. It should be the same as the cloning "
                   "factor of Fork. You may set the bands configuration by hand "
                   "with \"bands\" parameter, \"number\" will be ignored then.",
         std::to_string(kDefaultBandsNumber))
      TP("bands", "Bands configuration. Overrides \"number\".", "")
      TP("filter", "IIR filter type to apply.", kDefaultFilterType)
      TP("lengths", "IIR filter orders. \"auto\" for automatic selection.",
         "auto")
  )

  virtual void Initialize() const override;

 protected:
  static constexpr const char* kDefaultFilterType = "chebyshevII";
  static const int kDefaultBandsNumber;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*>* out) const noexcept override;
  void SetupFilter(size_t index, int frequency, IIRFilterBase* filter) const;

  int bands_number() const noexcept;

 private:
  mutable int bands_number_;
  mutable std::string bands_config_;
  std::string lengths_config_;
  mutable std::vector<int> lengths_;
  mutable std::vector<std::shared_ptr<IIRFilterBase>> filters_;
  std::string filter_type_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_TEMPLATE_TRANSFORM_H_
