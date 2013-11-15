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

namespace sound_feature_extraction {
namespace transforms {

//// @brief Applies a window function to each buffer.
class Window : public OmpUniformFormatTransform<formats::ArrayFormatF> {
  template <class T> friend class WindowSplitterTemplate;
  friend class WindowSplitter16;
  friend class WindowSplitterF;
 public:
  Window();

  TRANSFORM_INTRO("WindowFunction",
                  "Applies a window function to each window.",
                  Window)

  TP(type, WindowType, kDefaultType,
     "Type of the window. E.g. \"rectangular\" or \"hamming\".")
  TP(predft, bool, kDefaultPreDft,
     "Apply Discrete Fourier Transform to window function.")

  virtual void Initialize() const override;

 protected:
  typedef std::unique_ptr<float, void(*)(void*)> WindowContentsPtr;
  static constexpr WindowType kDefaultType = WINDOW_TYPE_HAMMING;
  static constexpr bool kDefaultPreDft = false;

  virtual void Do(const float* in,
                  float* out) const noexcept override;

  mutable WindowContentsPtr window_;

  static void ApplyWindow(bool simd, const float* window, int length,
                          const float* input, float* output) noexcept;

 private:
  static WindowContentsPtr InitializeWindow(size_t length,
                                            WindowType type,
                                            int allocSize = -1) noexcept;
};

}  // namespace transforms
}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORMS_WINDOW_H_
