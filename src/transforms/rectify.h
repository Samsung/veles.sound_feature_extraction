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

#include "src/formats/raw_format.h"
#include "src/formats/window_format.h"
#include "src/omp_transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class RectifyBase {
 protected:
  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;
};

template <class F>
class RectifyTemplate
    : public OmpUniformFormatTransform<F>,
      public RectifyBase {
 public:
  TRANSFORM_INTRO("Rectify", "Wave rectification to decrease high-frequency "
                             "content.")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual void Do(const typename F::BufferType& in,
                  typename F::BufferType* out) const noexcept {
    RectifyBase::Do(this->UseSimd(), in.Data.get(), this->inputFormat_->Size(),
                    out->Data.get());
  }
};

class RectifyWindow : public RectifyTemplate<Formats::WindowFormatF> {
 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept override;
};

class RectifyRaw : public RectifyTemplate<Formats::RawFormatF> {
 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::RawF>& in,
      BuffersBase<Formats::RawF>* buffers) const noexcept override;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_RECTIFY_H_
