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
#include "src/formats/format_limits.h"
#include "src/primitives/arithmetic-inl.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

const int RawToWindow::kDefaultLength = 25;
const int RawToWindow::kDefaultStep = 10;
const std::string RawToWindow::kDefaultType = "hamming";
const WindowType RawToWindow::kDefaultTypeEnum = WINDOW_TYPE_HAMMING;
const std::string Window::kDefaultType = "hamming";
const WindowType Window::kDefaultTypeEnum = WINDOW_TYPE_HAMMING;
const bool Window::kDefaultPreDft = false;

RawToWindow::RawToWindow()
  : step_(kDefaultStep),
    type_(kDefaultTypeEnum),
    outSizeEach_(0),
    inDataStep_(0) {
  outputFormat_->SetDuration(kDefaultLength);
  RegisterSetter("length", [&](const std::string& value) {
    int pv = Parse<int>("length", value);
    if (pv < MIN_WINDOW_DURATION || pv > MAX_WINDOW_DURATION) {
      return false;
    }
    outputFormat_->SetAllocatedSize(pv * outputFormat_->SamplingRate() / 1000);
    outputFormat_->SetDuration(pv);
    return true;
  });
  RegisterSetter("step", [&](const std::string& value) {
    int pv = Parse<int>("step", value);
    if (pv < MIN_WINDOW_STEP || pv > MAX_WINDOW_STEP) {
      return false;
    }
    step_ = pv;
    return true;
  });
  RegisterSetter("type", [&](const std::string& value) {
    auto wti = kWindowTypeMap.find(value);
    if (wti == kWindowTypeMap.end()) {
      return false;
    }
    type_ = wti->second;
    return true;
  });
}

void RawToWindow::OnInputFormatChanged() {
  outputFormat_->SetSamplingRate(inputFormat_->SamplingRate());
  // Allocate 2 extra samples to use zero-copy FFT
  outputFormat_->SetAllocatedSize(outputFormat_->Size() + 2);
}

void RawToWindow::Initialize() const noexcept {
  inDataStep_ = outputFormat_->Size();
  outSizeEach_ = inputFormat_->Size() / inDataStep_;
  if (inputFormat_->Size() % inDataStep_ != 0) {
    fprintf(stderr, "Input buffer size %zu is not divisible by step %i. "
            "It's excess will not be processed.\n",
            inputFormat_->Size(), inDataStep_);
  }

  window_ = Window::InitializeWindow(inDataStep_, type_);
}

void RawToWindow::InitializeBuffers(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Window16>* buffers) const noexcept {
  buffers->Initialize(in.Size() * outSizeEach_,
                      outputFormat_->AllocatedSize());
}

void RawToWindow::Do(const BuffersBase<Formats::Raw16>& in,
                     BuffersBase<Formats::Window16> *out)
const noexcept {
  BuffersBase<Formats::Window16>& outref = *out;
#ifdef __AVX__
  int16_t intbuf[inDataStep_] __attribute__ ((aligned (32)));  // NOLINT(*)
#endif
  float fbuf[inDataStep_] __attribute__ ((aligned (64)));  // NOLINT(*)
  float* window = window_.get();

  for (size_t i = 0; i < in.Size(); i++) {
    for (int step = 0; step < outSizeEach_; step++) {
      auto input = in[i]->Data.get() + step * inDataStep_;
      auto output = outref[step + i * outSizeEach_]->Data.get();
      if (type_ != WINDOW_TYPE_RECTANGULAR) {
#ifdef __AVX__
        if (align_complement_i16(input) != 0) {
          memcpy(intbuf, input, inDataStep_ * sizeof(int16_t));
          int16_to_float(intbuf, inDataStep_, fbuf);
        } else {
          int16_to_float(input, inDataStep_, fbuf);
        }
#else
        int16_to_float(input, inDataStep_, fbuf);
#endif
        Window::ApplyWindow(window, inDataStep_, fbuf, fbuf);
        float_to_int16(fbuf, inDataStep_, output);
      } else {  // type_ != WINDOW_TYPE_RECTANGULAR
        memcpy(output, input, inDataStep_ * sizeof(int16_t));
      }
    }
  }
}

Window::Window()
  : type_(kDefaultTypeEnum),
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

std::shared_ptr<float> Window::InitializeWindow(int length,
                                                WindowType type,
                                                int allocSize) noexcept {
  if (allocSize == -1) {
    allocSize = length;
  }

  auto window = std::shared_ptr<float>(
      mallocf(allocSize), [](float* ptr) {
    free(ptr);
  });

  auto windowContents = window.get();
  for (int i = 0; i < length; i++) {
    windowContents[i] = WindowElement(type, length, i);
  }

  return window;
}

void Window::ApplyWindow(const float* window, int length,
                         const float* input, float* output) noexcept {
#ifdef SIMD
  for (int i = 0; i < length - FLOAT_STEP + 1; i += FLOAT_STEP) {
    real_multiply(input + i, window + i, output + i);
  }
  for (int i = ((length >> FLOAT_STEP_LOG2) << FLOAT_STEP_LOG2);
      i < length; i++) {
    output[i] = input[i] * window[i];
  }
#else
  for (int i = 0; i < length; i++) {
    output[i] = input[i] * window[i];
  }
#endif
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

void Window::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size());
}

void Window::Do(const BuffersBase<Formats::WindowF>& in,
                BuffersBase<Formats::WindowF> *out)
const noexcept {
  float* window = window_.get();
  int length = inputFormat_->Size();
  for (size_t i = 0; i < in.Size(); i++) {
    auto input = in[i]->Data.get();
    auto output = (*out)[i]->Data.get();
    ApplyWindow(window, length, input, output);
  }
}

REGISTER_TRANSFORM(RawToWindow);
REGISTER_TRANSFORM(Window);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
