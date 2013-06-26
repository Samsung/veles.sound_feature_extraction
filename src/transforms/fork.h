/*! @file fork.h
 *  @brief Window cloning.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_FORK_H_
#define SRC_TRANSFORMS_FORK_H_

#include "src/formats/raw_format.h"
#include "src/transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class Fork
    : public UniformFormatTransform<Formats::RawFormatF> {
  friend class FrequencyBands;
 public:
  Fork();

  TRANSFORM_INTRO("Fork", "Clones the windows, increasing their number by "
                          "a factor of \"factor\".")

  TRANSFORM_PARAMETERS(
      TP("factor", "Windows number multiplier value.",
         std::to_string(kDefaultFactor))
  )

  static const std::string kCloningFactorAttributeName;

 protected:
  static const int kDefaultFactor;

  virtual BuffersCountChange OnFormatChanged() override;

  virtual void Do(const BuffersBase<float*>& in,
                  BuffersBase<float*>* out) const noexcept override;

 private:
  int factor_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_FORK_H_
