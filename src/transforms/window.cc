/*! @file window.cc
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

#include "src/transforms/window.h"
#include <fftf/api.h>
#include <simd/arithmetic-inl.h>

namespace sound_feature_extraction {
namespace transforms {

constexpr WindowType Window::kDefaultType;
constexpr bool Window::kDefaultPreDft;

Window::Window()
  : type_(kDefaultType),
    predft_(kDefaultPreDft),
    window_(nullptr, free) {
}

ALWAYS_VALID_TP(Window, type)
ALWAYS_VALID_TP(Window, predft)

Window::WindowContentsPtr Window::InitializeWindow(size_t length,
                                                   WindowType type,
                                                   int allocSize) noexcept {
  if (allocSize == -1) {
    allocSize = length;
  }

  auto window = WindowContentsPtr(mallocf(allocSize), free);

  auto windowContents = window.get();
  for (int i = 0; i < static_cast<int>(length); i++) {
    windowContents[i] = WindowElement(type, length, i);
  }

  return window;
}

void Window::ApplyWindow(bool simd, const float* window, int length,
                         const float* input, float* output) noexcept {
  if (simd) {
#ifdef SIMD
    for (int i = 0; i < length - FLOAT_STEP + 1; i += FLOAT_STEP) {
      real_multiply(input + i, window + i, output + i);
    }
    for (int i = ((length >> FLOAT_STEP_LOG2) << FLOAT_STEP_LOG2);
        i < length; i++) {
      output[i] = input[i] * window[i];
    }
  } else {
#else
  } {
#endif
    for (int i = 0; i < length; i++) {
      output[i] = input[i] * window[i];
    }
  }
}

void Window::Initialize() const {
  if (!predft_) {
    window_ = InitializeWindow(input_format_->Size(), type_);
  } else {
    int length = input_format_->Size();
    int fftLength = (length - 2) / 2;
    window_ = InitializeWindow(fftLength, type_, length);
    auto fftPlan = std::unique_ptr<FFTFInstance, void (*)(FFTFInstance *)>(
        fftf_init(
            FFTF_TYPE_REAL, FFTF_DIRECTION_FORWARD,
            FFTF_DIMENSION_1D,
            &fftLength, FFTF_NO_OPTIONS,
            window_.get(), window_.get()),
        fftf_destroy);

    fftf_calc(fftPlan.get());
  }
}

void Window::Do(const float* in,
                float* out)
const noexcept {
  ApplyWindow(use_simd(), window_.get(), input_format_->Size(), in, out);
}

RTP(Window, type)
RTP(Window, predft)
REGISTER_TRANSFORM(Window);

}  // namespace transforms
}  // namespace sound_feature_extraction
