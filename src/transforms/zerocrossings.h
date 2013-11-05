/*! @file zerocrossings.h
 *  @brief Number of time domain zero crossings of the signal.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_ZEROCROSSINGS_H_
#define SRC_TRANSFORMS_ZEROCROSSINGS_H_

#include "src/formats/array_format.h"
#include "src/formats/single_format.h"
#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

template <class F>
class ZeroCrossingsTemplate
    : public OmpTransformBase<F, formats::SingleFormat32> {
 public:
  TRANSFORM_INTRO("ZeroCrossings", "Number of time domain zero crossings "
                                   "of the signal.")

  OMP_TRANSFORM_PARAMETERS()

 protected:
  virtual void Do(const typename F::BufferElementType* in,
                  int32_t* out) const noexcept override final {
    auto result = DoInternal(this->UseSimd(), in,
                             this->input_format_->Size());
    assert(result >= 0 &&
           result <= static_cast<int>(this->input_format_->Size()));
    *out = result;
  }

  virtual int DoInternal(bool simd, const typename F::BufferElementType* input,
                         size_t length) const noexcept = 0;
};

class ZeroCrossingsF
    : public ZeroCrossingsTemplate<formats::ArrayFormatF> {
 protected:
  virtual int DoInternal(bool simd, const float* input, size_t length)
      const noexcept override;
};

class ZeroCrossings16
    : public ZeroCrossingsTemplate<formats::ArrayFormat16> {
 protected:
  virtual int DoInternal(bool simd, const int16_t* input, size_t length)
      const noexcept override;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_ZEROCROSSINGS_H_
