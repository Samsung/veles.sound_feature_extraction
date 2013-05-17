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
#include <simd/arithmetic-inl.h>

namespace SoundFeatureExtraction {
namespace Transforms {

const int RawToWindow::kDefaultLength = DEFAULT_WINDOW_SAMPLES;
const int RawToWindow::kDefaultStep = DEFAULT_WINDOW_STEP;
const std::string RawToWindow::kDefaultType = "hamming";
const WindowType RawToWindow::kDefaultTypeEnum = WINDOW_TYPE_HAMMING;
const std::string Window::kDefaultType = "hamming";
const WindowType Window::kDefaultTypeEnum = WINDOW_TYPE_HAMMING;
const bool Window::kDefaultPreDft = false;

RawToWindow::RawToWindow()
  : window_(nullptr, free),
    step_(kDefaultStep),
    type_(kDefaultTypeEnum),
    windowsCount_(0) {
  outputFormat_->SetDuration(kDefaultLength * 1000
                             / outputFormat_->SamplingRate());
  RegisterSetter("length", [&](const std::string& value) {
    int pv = Parse<int>("length", value);
    if (pv < MIN_WINDOW_SAMPLES || pv > MAX_WINDOW_SAMPLES) {
      return false;
    }
    outputFormat_->SetAllocatedSize(pv);
    outputFormat_->SetDuration(pv * 1000 / outputFormat_->SamplingRate());
    return true;
  });
  RegisterSetter("step", [&](const std::string& value) {
    int pv = Parse<int>("step", value);
    if (pv < 1) {
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
  outputFormat_->SetParentRawSize(inputFormat_->Size());
}

void RawToWindow::OnOutputFormatChanged() {
  inputFormat_->SetSamplingRate(outputFormat_->SamplingRate());
  inputFormat_->SetSize(outputFormat_->ParentRawSize());
}

void RawToWindow::Initialize() const noexcept {
  int realSize = inputFormat_->Size() - outputFormat_->Size();
  windowsCount_ = realSize / step_ + 1;
  int excess = realSize % step_;
  if (excess != 0) {
    WRN("(input buffer size %zu - window length %zu) = %i is not "
        "divisible by step %i. It's excess (%i samples) will not be "
        "processed.\n",
        inputFormat_->Size(), outputFormat_->Size(),
        realSize, step_, excess);
  }

  window_ = Window::InitializeWindow(outputFormat_->Size(), type_);
}

void RawToWindow::InitializeBuffers(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Window16>* buffers) const noexcept {
  buffers->Initialize(in.Size() * windowsCount_,
                      outputFormat_->AllocatedSize());
}

void RawToWindow::InitializeBuffers(
    const BuffersBase<Formats::Window16>& in,
    BuffersBase<Formats::Raw16>* buffers) const noexcept {
  buffers->Initialize(in.Size() / windowsCount_, inputFormat_->Size(), 0);
}

void RawToWindow::Do(const BuffersBase<Formats::Raw16>& in,
                     BuffersBase<Formats::Window16> *out)
const noexcept {
  BuffersBase<Formats::Window16>& outref = *out;
#ifdef __AVX__
  int16_t intbuf[outputFormat_->Size()] __attribute__ ((aligned (32)));  // NOLINT(*)
#endif
  float fbuf[outputFormat_->Size()] __attribute__ ((aligned (64)));  // NOLINT(*)
  float* window = window_.get();

  for (size_t i = 0; i < in.Size(); i++) {
    for (int j = 0; j < windowsCount_; j++) {
      auto input = in[i]->Data.get() + j * step_;
      auto output = outref[i * windowsCount_ + j]->Data.get();
      if (type_ != WINDOW_TYPE_RECTANGULAR) {
#ifdef __AVX__
        if (align_complement_i16(input) != 0) {
          memcpy(intbuf, input, outputFormat_->Size() * sizeof(int16_t));
          int16_to_float(intbuf, outputFormat_->Size(), fbuf);
        } else {
          int16_to_float(input, outputFormat_->Size(), fbuf);
        }
#else
        int16_to_float(input, outputFormat_->Size(), fbuf);
#endif
        Window::ApplyWindow(true, window, outputFormat_->Size(), fbuf, fbuf);
        float_to_int16(fbuf, outputFormat_->Size(), output);
      } else {  // type_ != WINDOW_TYPE_RECTANGULAR
        memcpy(output, input, outputFormat_->Size() * sizeof(int16_t));
      }
    }
  }
}

void RawToWindow::Do(const BuffersBase<Formats::Window16>& in,
                     BuffersBase<Formats::Raw16> *out) const noexcept {
  int windowLength = outputFormat_->Size();
  int offset = (windowLength - step_) / 2;
  int rawIndex = 0;
  size_t skippedEndingSize = inputFormat_->Size() -
      step_ * (windowsCount_ - 1) - outputFormat_->Size();
  for (size_t i = 0; i < in.Size(); i++) {
    int windowIndex = i % windowsCount_;
    if (windowIndex == 0) {
      memcpy((*out)[rawIndex]->Data.get(), in[i]->Data.get(),
             (windowLength - offset) * sizeof(int16_t));
    } else if (windowIndex < windowsCount_ - 1) {
      memcpy((*out)[rawIndex]->Data.get() +
                 windowLength - offset + step_ * (windowIndex - 1),
             in[i]->Data.get() + offset, step_ * sizeof(int16_t));
    } else {
      memcpy((*out)[rawIndex]->Data.get() +
                 windowLength - offset + step_ * (windowIndex - 1),
             in[i]->Data.get() + offset,
             (windowLength - offset) * sizeof(int16_t));
      memset((*out)[rawIndex]->Data.get() +
                 inputFormat_->Size() - skippedEndingSize,
             0, skippedEndingSize * sizeof(int16_t));
      rawIndex++;
    }
  }
}

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
    ApplyWindow(true, window, length, input, output);
  }
}

REGISTER_TRANSFORM(RawToWindow);
REGISTER_TRANSFORM(Window);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
