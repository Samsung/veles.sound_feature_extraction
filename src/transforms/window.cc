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
#include <string>
#include "src/formats/format_limits.h"
#include "src/primitives/arithmetic-inl.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

Window::Window()
: TransformBase(SupportedParameters())
, step_(0)
, type_(WINDOW_TYPE_HAMMING)
, outSizeEach_(0)
, inDataStep_(0) {
}

void Window::OnInputFormatChanged() {
  outputFormat_->SetSamplingRate(inputFormat_->SamplingRate());
  // Allocate 2 extra samples to use zero-copy FFT
  outputFormat_->SetAllocatedSize(outputFormat_->Size() + 2);
}

void Window::SetParameter(const std::string& name,
                          const std::string& value) {
  if (name == "length") {
    int pv = Parse<int>(name, value);
    if (pv < MIN_WINDOW_DURATION || pv > MAX_WINDOW_DURATION) {
      throw InvalidParameterValueException(name, value, Name());
    }
    outputFormat_->SetAllocatedSize(pv * outputFormat_->SamplingRate() / 1000);
    outputFormat_->SetDuration(pv);
  } else if (name == "step") {
    int pv = Parse<int>(name, value);
    if (pv < MIN_WINDOW_STEP || pv > MAX_WINDOW_STEP) {
      throw InvalidParameterValueException(name, value, Name());
    }
    step_ = pv;
  } else if (name == "window") {
    auto wti = WindowTypeMap.find(value);
    if (wti == WindowTypeMap.end()) {
      throw InvalidParameterValueException(name, value, Name());
    }
    type_ = wti->second;
  }
}

void Window::Initialize() const noexcept {
  inDataStep_ = outputFormat_->Size();
  outSizeEach_ = inputFormat_->Size() / inDataStep_;
  if (inputFormat_->Size() % inDataStep_ != 0) {
    fprintf(stderr, "Input buffer size %zu is not divisible by step %i.\n",
            inputFormat_->Size(), inDataStep_);
  }
}

void Window::InitializeBuffers(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Window16>* buffers) const noexcept {
  buffers->Initialize(in.Size() * outSizeEach_,
                      outputFormat_->AllocatedSize());
  window_ = std::shared_ptr<float>(
      mallocf(inDataStep_), [](float* ptr) {
    free(ptr);
  });
  auto window = window_.get();
  for (int i = 0; i < inDataStep_; i++) {
    window[i] = WindowElement(type_, inDataStep_, i);
  }
}

void Window::Do(const BuffersBase<Formats::Raw16>& in,
                        BuffersBase<Formats::Window16> *out)
const noexcept {
  BuffersBase<Formats::Window16>& outref = *out;
  int16_t intbuf[inDataStep_] __attribute__ ((aligned (64)));  // NOLINT(*)
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
#ifdef SIMD
        for (int i = 0; i < inDataStep_ - FLOAT_STEP + 1; i += FLOAT_STEP) {
          real_multiply(&fbuf[i], window + i, &fbuf[i]);
        }
        for (int i = (((inDataStep_) >> FLOAT_STEP_LOG2) << FLOAT_STEP_LOG2);
            i < inDataStep_; i++) {
          fbuf[i] *= window[i];
        }
#else
        for (int i = 0; i < inDataStep_; i++) {
          fbuf[i] *= window[i];
        }
#endif
        float_to_int16(fbuf, inDataStep_, output);
      } else {  // type_ != WINDOW_TYPE_RECTANGULAR
        memcpy(output, input, inDataStep_ * sizeof(int16_t));
      }
    }
  }
}

REGISTER_TRANSFORM(Window);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
