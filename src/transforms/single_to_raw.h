/*! @file single_to_raw.h
 *  @brief Merge all single-s to one solid array.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_SINGLE_TO_RAW_H_
#define SRC_TRANSFORMS_SINGLE_TO_RAW_H_

#include "src/transform_base.h"
#include "src/formats/single_format.h"
#include "src/formats/array_format.h"

namespace sound_feature_extraction {
namespace transforms {

template <class T>
class SingleToRaw
    : public TransformBase<formats::SingleFormat<T>,
                           formats::ArrayFormat<T>>{
 public:
  TRANSFORM_INTRO("Merge", "Merge all single-s to one solid array.")

  TRANSFORM_PARAMETERS()

 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override final {
    this->outputFormat_->SetSize(buffersCount);
    return 1;
  }

  virtual void Do(const BuffersBase<T>& in,
                  BuffersBase<T*>* out) const noexcept override {
    for (size_t i = 0; i < in.Count(); i++) {
      (*out)[0][i] = in[i];
    }
  }
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_SINGLE_TO_RAW_H_
