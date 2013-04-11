/*! @file rectify.h
 *  @brief Wave rectification.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_RECTIFY_H_
#define SRC_TRANSFORMS_RECTIFY_H_

#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class Rectify
    : public UniformFormatTransform<Formats::WindowFormatF> {
 public:

  TRANSFORM_INTRO("Rectify", "Wave rectification")

  TRANSFORM_PARAMETERS()

 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                  BuffersBase<Formats::WindowF>* out) const noexcept;

  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_RECTIFY_H_
