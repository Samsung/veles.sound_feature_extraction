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
#include "src/transform_base.h"
#include "src/primitives/window.h"

namespace SoundFeatureExtraction {
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
  typedef std::unique_ptr<float, void(*)(void*)> WindowContentsPtr;

  virtual void InitializeBuffers(
      const BuffersBase<Formats::WindowF>& in,
      BuffersBase<Formats::WindowF>* buffers) const noexcept;

  virtual void Do(const BuffersBase<Formats::WindowF>& in,
                  BuffersBase<Formats::WindowF> *out) const noexcept;

  mutable WindowContentsPtr window_;

  static void ApplyWindow(bool simd, const float* window, int length,
                          const float* input, float* output) noexcept;

 private:
  static const std::string kDefaultType;
  static const WindowType kDefaultTypeEnum;
  static const bool kDefaultPreDft;

  WindowType type_;
  bool preDft_;

  static WindowContentsPtr InitializeWindow(size_t length,
                                            WindowType type,
                                            int allocSize = -1) noexcept;
};

/// @brief Splits the raw stream into numerous small chunks aka windows.
class RawToWindow
    : public TransformBase<Formats::RawFormat16, Formats::WindowFormat16, true>,
      public TransformLogger<RawToWindow> {
 public:
  RawToWindow();

  TRANSFORM_INTRO("Window", "Splits the raw input signal into numerous "
                            "windows stepping \"step\" ms with length "
                            "\"length\" ms of type \"type\".")

  TRANSFORM_PARAMETERS(
      TP("length", "Window size in samples.",
         std::to_string(kDefaultLength))
      TP("step", "Distance between sequential windows in samples.",
         std::to_string(kDefaultStep))
      TP("type", "Type of the window. E.g. \"rectangular\" "
                 "or \"hamming\".",
         kDefaultType)
  )

  virtual void Initialize() const noexcept;

 protected:
  virtual void OnInputFormatChanged();
  virtual void OnOutputFormatChanged();

  virtual void InitializeBuffers(const BuffersBase<Formats::Raw16>& in,
        BuffersBase<Formats::Window16>* buffers) const noexcept;

  virtual void InitializeBuffers(const BuffersBase<Formats::Window16>& in,
        BuffersBase<Formats::Raw16>* buffers) const noexcept;

  virtual void Do(const BuffersBase<Formats::Raw16>& in,
                  BuffersBase<Formats::Window16> *out) const noexcept;

  virtual void Do(const BuffersBase<Formats::Window16>& in,
                  BuffersBase<Formats::Raw16> *out) const noexcept;

  mutable Window::WindowContentsPtr window_;

 private:
  static const int kDefaultLength;
  static const int kDefaultStep;
  static const std::string kDefaultType;
  static const WindowType kDefaultTypeEnum;

  int step_;
  WindowType type_;
  mutable int windowsCount_;
};

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_WINDOW_H_
