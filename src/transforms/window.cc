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

namespace SoundFeatureExtraction {
namespace Transforms {

const std::string Window::kDefaultType = "hamming";
const WindowType Window::kDefaultTypeEnum = WINDOW_TYPE_HAMMING;
const bool Window::kDefaultPreDft = false;

Window::Window()
  : window_(nullptr, free),
    type_(kDefaultTypeEnum),
    preDft_(kDefaultPreDft) {
  RegisterSetter("type", [&](const std::string& value) {
    auto wti = kWindowTypeMap.find(value);
    if (wti == kWindowTypeMap.end()) {
      return false;
    }
    type_ = wti->second;
    return true;
  });
  RegisterSetter("predft", [&](const std::string& value) {
    preDft_ = Parse<bool>("predft", value);
    return true;
  });
}

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

  return std::move(window);
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

void Window::Initialize() const noexcept {
  if (!preDft_) {
    window_ = InitializeWindow(inputFormat_->Size(), type_);
  } else {
    int length = inputFormat_->Size();
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
  ApplyWindow(UseSimd(), window_.get(), inputFormat_->Size(), in, out);
}

REGISTER_TRANSFORM(Window);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
