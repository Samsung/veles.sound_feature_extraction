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

#include "src/formats/raw_format.h"
#include "src/formats/window_format.h"
#include "src/uniform_format_transform.h"
#include "src/primitives/window.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

//// @brief Applies a window function to each window.
class Window
    : public UniformFormatTransform<Formats::WindowFormatF> {
  friend class RawToWindow;
 public:
  Window();

  TRANSFORM_INTRO("Window", "Applies a window function to each window.")

  TRANSFORM_PARAMETERS(
      TP("type", "Type of the window. E.g. \"rectangular\" "
                 "or \"hamming\".",
         kDefaultType)
      TP("predft", "Apply Discrete Fourier Transform to window function.",
         std::to_string(kDefaultPreDft))
  )

  virtual void Initialize() const noexcept;

 protected:
  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                  BuffersBase<Formats::WindowF> *out) const noexcept;

 private:
  typedef std::unique_ptr<float, void(*)(void*)> WindowContentsPtr;

  static const std::string kDefaultType;
  static const WindowType kDefaultTypeEnum;
  static const bool kDefaultPreDft;

  WindowType type_;
  bool preDft_;
  mutable WindowContentsPtr window_;

  static WindowContentsPtr InitializeWindow(int length,
                                            WindowType type,
                                            int allocSize = -1) noexcept;

  static void ApplyWindow(const float* window, int length,
                          const float* input, float* output) noexcept;
};

/// @brief Splits the raw stream into numerous small chunks aka windows.
class RawToWindow
    : public TransformBase<Formats::RawFormat16, Formats::WindowFormat16> {
 public:
  RawToWindow();

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
  static const WindowType kDefaultTypeEnum;

  int step_;
  WindowType type_;
  mutable int outSizeEach_;
  mutable int inDataStep_;
  mutable Window::WindowContentsPtr window_;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_WINDOW_H_
