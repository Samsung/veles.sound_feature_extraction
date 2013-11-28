/*! @file rotate.h
 *  @brief Rotate the buffers.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_ROTATE_H_
#define SRC_TRANSFORMS_ROTATE_H_

#include "src/transforms/common.h"

namespace sound_feature_extraction {
namespace transforms {

template <class T>
class Rotate : public UniformFormatOmpAwareTransform<formats::ArrayFormat<T>> {
 public:
  TRANSFORM_INTRO("Rotate", "Rotate the buffers, so that all elements with the "
                  "same index get to the same buffer.",
                  Rotate)

 protected:
  virtual size_t OnFormatChanged(size_t buffersCount) override final {
    this->output_format_->SetSize(buffersCount);
    return this->input_format_->Size();
  }

  virtual void Do(const BuffersBase<T*>& in,
                  BuffersBase<T*>* out) const noexcept override {
#ifdef HAVE_OPENMP
    #pragma omp parallel for num_threads(this->threads_number())
#endif
    for (size_t i = 0; i < this->input_format_->Size(); i++) {
      for (size_t j = 0; j < in.Count(); j++) {
        (*out)[i][j] = in[j][i];
      }
    }
  }
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_ROTATE_H_
