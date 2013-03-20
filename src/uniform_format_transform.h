/*! @file uniform_format_transform.h
 *  @brief Common base of transforms which do not change the buffer format.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_UNIFORM_FORMAT_TRANSFORM_H_
#define SRC_UNIFORM_FORMAT_TRANSFORM_H_

#include <string>
#include "src/transform_base.h"

namespace SpeechFeatureExtraction {

/// @brief Common base of transforms which do not change the buffer format.
template <typename F>
class UniformFormatTransform : public TransformBase<F, F> {
 public:
  virtual ~UniformFormatTransform() {}

 protected:
  virtual void OnInputFormatChanged() {
    TransformBase<F, F>::outputFormat_ = std::make_shared<F>(
        *TransformBase<F, F>::inputFormat_);
    OnFormatChanged();
  }

  virtual void OnFormatChanged() {
  }
};

}  // namespace SpeechFeatureExtraction

#endif  // SRC_UNIFORM_FORMAT_TRANSFORM_H_
