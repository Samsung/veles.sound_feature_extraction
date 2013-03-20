/*! @file intensity.h
 *  @brief Sound intensity calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_INTENSITY_H_
#define SRC_TRANSFORMS_INTENSITY_H_

#include <string>
#include "src/formats/window_format.h"
#include "src/formats/single_format.h"
#include "src/transform_base.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class Intensity
    : public TransformBase<Formats::WindowFormatF, Formats::SingleFormat> {
 public:
  Intensity();

  TRANSFORM_INTRO("Intensity", "Unnormalized sound intensity calculation.")

  TRANSFORM_PARAMETERS()

 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<float>* buffers) const noexcept;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                          BuffersBase<float> *out) const noexcept;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_INTENSITY_H_
