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

#include "src/transforms/common.h"
#include "src/primitives/window.h"

namespace SoundFeatureExtraction {
namespace Transforms {

//// @brief Applies a window function to each buffer.
class Window : public OmpUniformFormatTransform<Formats::ArrayFormatF> {
  template <class T> friend class WindowSplitterTemplate;
  friend class WindowSplitter16;
  friend class WindowSplitterF;
 public:
  Window();

  TRANSFORM_INTRO("WindowFunction",
                  "Applies a window function to each window.")

  TRANSFORM_PARAMETERS(
      TP("type", "Type of the window. E.g. \"rectangular\" "
                 "or \"hamming\".",
         kDefaultType)
      TP("predft", "Apply Discrete Fourier Transform to window function.",
         std::to_string(kDefaultPreDft))
  )

  virtual void Initialize() const override;

 protected:
  typedef std::unique_ptr<float, void(*)(void*)> WindowContentsPtr;

  virtual void Do(const float* in,
                  float* out) const noexcept override;

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

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORMS_WINDOW_H_
