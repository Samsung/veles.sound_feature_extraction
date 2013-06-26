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
#include "src/formats/raw_format.h"
#include "src/transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class FrequencyBands
    : public UniformFormatTransform<Formats::RawFormatF>,
      public TransformLogger<FrequencyBands> {
 public:
  FrequencyBands();

  TRANSFORM_INTRO("FrequencyBands", "Cut the frequency bands from sequential windows.")

  TRANSFORM_PARAMETERS(
      TP("number", "The number of bands. It should be the same as the cloning "
                   "factor of Fork. You may set the bands configuration by hand "
                   "with \"bands\" parameter, \"number\" will be discarded then.",
         std::to_string(kDefaultBandsNumber))
      TP("bands", "Bands configuration. Overrides \"number\".",
         kDefaultBandsConfig)
  )

  virtual void Initialize() const noexcept override;

 protected:
  static const int kDefaultBandsNumber;
  static const std::string kDefaultBandsConfig;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*>* out) const noexcept override;

 private:
  int bandsNumber_;
  std::string bandsConfig_;
  mutable std::vector<int> bands_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_TEMPLATE_TRANSFORM_H_
