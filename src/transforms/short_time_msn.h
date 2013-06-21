/*! @file short_time_msn.h
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_SHORT_TIME_MSN_H_
#define SRC_TRANSFORMS_SHORT_TIME_MSN_H_

#include "src/formats/window_format.h"
#include "src/transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class ShortTimeMeanScaleNormalization
    : public UniformFormatTransform<Formats::WindowFormatF> {
 public:
  ShortTimeMeanScaleNormalization();

  TRANSFORM_INTRO("STMSN", "Calculate short-time mean and scale normalized "
                           "values, that is,"
                           "$stmsn_n[i] = \frac{w_n[i] - "
                           "\\sum_{k=n-L/2}^{n+L/2}{w_k[i]}}"
                           "{\\max_{k=n-L/2}^{n+L/2}{w_k[i]} -"
                           "\\min_{k=n-L/2}^{n+L/2}{w_k[i]}}$.")

  TRANSFORM_PARAMETERS(
      TP("length", "The amount of local values to average.",
         std::to_string(kDefaultLength))
  )

 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept override;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                  BuffersBase<Formats::WindowF>* out) const noexcept override;

 private:
  static const int kDefaultLength = 300;

  int length_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_SHORT_TIME_MSN_H_
