/*! @file window_transform.h
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

#ifndef WINDOWTRANSFORM_H_
#define WINDOWTRANSFORM_H_

#include "src/formats/raw_format.h"
#include "src/formats/raw_window_format.h"
#include "src/transform_base.h"

namespace SpeechFeatureExtraction {

class WindowTransform
    : public TransformBase<Formats::RawFormat, Formats::RawWindowFormat> {
 public:
  WindowTransform() : TransformBase(SupportedParameters()) {}

  TRANSFORM_NAME("!Root")

  TRANSFORM_PARAMETERS(
      _TP_("length", "Window size in milliseconds", "25")
      _TP_("step", "Distance between sequential windows in milliseconds", "10")
  )

  virtual void Initialize() const noexcept {
  }

  virtual void Do(const Buffers& in, Buffers *out) const noexcept {
    CopyInToOut(in, out);
  }

 protected:
  virtual void SetParameter(const std::string& name, const std::string& value);
};

}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_WINDOWTRANSFORM_H_
