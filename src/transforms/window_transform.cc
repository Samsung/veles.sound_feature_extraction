/*! @file window_transform.cc
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/window_transform.h"
#include "src/formats/format_limits.h"
#include "src/primitives/arithmetic.h"

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
  outputFormat_.SetSamplingRate(inputFormat_.SamplingRate());
}

void Window::SetParameter(const std::string& name,
                                   const std::string& value) {
  if (name == "length") {
    int pv = Parse<int>(name, value);
    if (pv < MIN_WINDOW_DURATION || pv > MAX_WINDOW_DURATION) {
      throw new InvalidParameterValueException(name, value, Name());
    }
    outputFormat_.SetDuration(pv);
  } else if (name == "step") {
    int pv = Parse<int>(name, value);
    if (pv < MIN_WINDOW_STEP || pv > MAX_WINDOW_STEP) {
      throw new InvalidParameterValueException(name, value, Name());
    }
    step_ = pv;
  } else if (name == "window") {
    auto wti = WindowTypeMap.find(value);
    if (wti == WindowTypeMap.end()) {
      throw new InvalidParameterValueException(name, value, Name());
    }
    type_ = wti->second;
  }
}

void Window::Initialize() const noexcept {
  inDataStep_ = outputFormat_.SamplesCount();
  outSizeEach_ = inputFormat_.Size() / inDataStep_;
  if (inputFormat_.Size() % inDataStep_ != 0) {
    fprintf(stderr, "Input buffer size %zu is not divisible by step %i",
            inputFormat_.Size(), inDataStep_);
  }
}

void Window::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Window16>* buffers) const noexcept {
  // Adding 2 extra samples here to be compatible with real DFT
  buffers->Initialize(in.Size() * outSizeEach_, inDataStep_ + 2);
  window_ = std::shared_ptr<float>(
      mallocf(inDataStep_), [](float* ptr) {
    free(ptr);
  });
  float* window = window_.get();
  for (int i = 0; i < inDataStep_; i++) {
    window[i] = WindowElement(type_, i, inDataStep_);
  }
}

void Window::TypeSafeDo(const BuffersBase<Formats::Raw16>& in,
                        BuffersBase<Formats::Window16> *out)
const noexcept {
  BuffersBase<Formats::Window16>& outref = *out;
  int16_t intbuf[inDataStep_] __attribute__ ((aligned (64)));
  float fbuf[inDataStep_] __attribute__ ((aligned (64)));
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
        for (int i = 0; i < inDataStep_ - 7; i += 8) {
          real_multiply(&fbuf[i], window + i, &fbuf[i]);
        }
        for (int i = (((inDataStep_) >> 3) << 3); i < inDataStep_; i++) {
          fbuf[i] *= window[i];
        }
        float_to_int16(fbuf, inDataStep_, output);
#elif defined (__ARM_NEON__)
        int16_to_float(input, inDataStep_, fbuf);
        for (int i = 0; i < inDataStep_ - 3; i += 4) {
          real_multiply(fbuf + i, window + i, fbuf + i);
        }
        for (int i = (inDataStep_ >> 2) << 2; i < inDataStep_; i++) {
          fbuf[i] *= window[i];
        }
        float_to_int16(fbuf, inDataStep_, output);
#else
        int16_to_float(input, inDataStep_, fbuf);
        for (int i = 0; i < inDataStep_; i++) {
          fbuf[i] *= window[i];
        }
        float_to_int16(fbuf, inDataStep_, output);
#endif
      } else {  // type_ != WINDOW_TYPE_RECTANGULAR
        memcpy(output, input, inDataStep_ * sizeof(int16_t));
      }
    }
  }
}

REGISTER_TRANSFORM(Window);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
