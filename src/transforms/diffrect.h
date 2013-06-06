/*! @file diffrect.h
 *  @brief Find the difference from one sample to the next.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_DIFFRECT_H_
#define SRC_TRANSFORMS_DIFFRECT_H_

#include "src/formats/raw_format.h"
#include "src/formats/window_format.h"
#include "src/omp_transform_base.h"

namespace SoundFeatureExtraction {
namespace Transforms {

class DiffrectBase {
 protected:
  static void Do(bool simd, const float* input, int length,
                 float* output) noexcept;
};

template <class F>
class DiffrectTemplate
    : public OmpUniformFormatTransform<F>,
      public DiffrectBase {
 public:
  TRANSFORM_INTRO("Diffrect", "Find the difference from one sample to the next.")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual void OnFormatChanged() {
    this->outputFormat_->SetSize(this->inputFormat_->Size() - 1);
  }

  virtual void Do(const typename F::BufferType& in,
                  typename F::BufferType* out) const noexcept {
    DiffrectBase::Do(true, in.Data.get(), this->inputFormat_->Size(),
                     out->Data.get());
  }
};

class DiffrectWindow
    : public DiffrectTemplate<Formats::WindowFormatF> {
 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;
};

class DiffrectRaw
    : public DiffrectTemplate<Formats::RawFormatF> {
 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::RawF>& in,
      BuffersBase<Formats::RawF>* buffers) const noexcept;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_DIFFRECT_H_
