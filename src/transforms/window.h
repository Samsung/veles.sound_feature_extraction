/*! @file window.h
 *  @brief Windowing transform.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_WINDOW_H_
#define SRC_TRANSFORMS_WINDOW_H_

#include <string>
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
      TP("length", "Window size in milliseconds",
         std::to_string(kDefaultLength))
      TP("step", "Distance between sequential windows in milliseconds",
         std::to_string(kDefaultStep))
      TP("type", "Type of the window. E.g. \"rectangular\" "
                 "or \"hamming\".",
         kDefaultType)
  )

  virtual void Initialize() const noexcept;

 protected:
  virtual void OnInputFormatChanged();

  virtual void InitializeBuffers(const BuffersBase<Formats::Raw16>& in,
        BuffersBase<Formats::Window16>* buffers) const noexcept;

  virtual void Do(const BuffersBase<Formats::Raw16>& in,
                          BuffersBase<Formats::Window16> *out) const noexcept;

 private:
  static const int kDefaultLength;
  static const int kDefaultStep;
  static const std::string kDefaultType;

  int step_;
  WindowType type_;
  mutable std::shared_ptr<float> window_;
  mutable int outSizeEach_;
  mutable int inDataStep_;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_WINDOW_H_
