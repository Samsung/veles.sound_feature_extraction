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
#include "src/formats/window_format.h"
#include "src/transform_base.h"
#include "src/primitives/window.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class Window
    : public TransformBase<Formats::RawFormat16, Formats::WindowFormat16> {
 public:
  Window();

  TRANSFORM_INTRO("Window", "Splits the raw input signal into numerous "
                            "windows stepping \"step\" ms with length "
                            "\"length\" ms of type \"type\".")

  TRANSFORM_PARAMETERS(
      _TP_("length", "Window size in milliseconds", "25")
      _TP_("step", "Distance between sequential windows in milliseconds", "10")
      _TP_("type", "Type of the window. Supported values are \"rectangular\" "
           "and \"hamming\".", "hamming")
  )

  virtual void Initialize() const noexcept;

 protected:
  virtual void OnInputFormatChanged();

  virtual void SetParameter(const std::string& name, const std::string& value);

  virtual void TypeSafeInitializeBuffers(const BuffersBase<Formats::Raw16>& in,
        BuffersBase<Formats::Window16>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Formats::Raw16>& in,
                          BuffersBase<Formats::Window16> *out) const noexcept;

 private:
  int step_;
  WindowType type_;
  mutable std::shared_ptr<float> window_;
  mutable int outSizeEach_;
  mutable int inDataStep_;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_WINDOWTRANSFORM_H_
