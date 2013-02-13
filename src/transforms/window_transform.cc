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

namespace SpeechFeatureExtraction {
namespace Transforms {

Window::Window()
: TransformBase(SupportedParameters())
, step_(0) {
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
  }
}

void Window::Initialize() const noexcept {
  inDataStep_ = inputFormat_.SamplingRate() * step_;
  outSizeEach_ = inputFormat_.Size() / inDataStep_;
  if (inputFormat_.Size() % inDataStep_ != 0) {
    fprintf(stderr, "Input buffer size %zu is not divisible by step %i",
            inputFormat_.Size(), inDataStep_);
  }
}

int Window::BuffersCountMultiplier() const noexcept {
  return outSizeEach_;
}

void Window::TypeSafeDo(const BuffersBase<Formats::Raw>& in,
                                 BuffersBase<Formats::Window> *out)
const noexcept {
  BuffersBase<Formats::Window>& outref = *out;
  for (int i = 0; i < in.Size(); i++) {
    for (int step = 0; step < outSizeEach_; step++) {
      *outref[step + i * outSizeEach_] = { in[i]->Data + step * inDataStep_ };
    }
  }
}

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
