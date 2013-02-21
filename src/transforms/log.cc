/*! @file log.cc
 *  @brief Taking logarithm on the values.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/log.h"
#include <math.h>

namespace SpeechFeatureExtraction {
namespace Transforms {

const std::unordered_map<std::string, Log::LogBase> Log::LogBaseMap {
  { "e", LOG_BASE_E },
  { "2", LOG_BASE_2 },
  { "10", LOG_BASE_10 }
};

Log::Log()
: UniformFormatTransform(SupportedParameters())
, base_(LOG_BASE_E) {
}

bool Log::HasInverse() const noexcept {
  return true;
}

void Log::SetParameter(const std::string& name,
                       const std::string& value) {
  if (name == "base") {
    auto lbit = LogBaseMap.find(value);
    if (lbit == LogBaseMap.end()) {
      throw new InvalidParameterValueException(name, value, Name());
    }
    base_ = lbit->second;
  }
}

void Log::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_.SamplesCount());
}

void Log::TypeSafeDo(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF> *out) const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    auto input = in[i]->Data.get();
    auto output = (*out)[i]->Data.get();
    for (size_t j = 0; j < inputFormat_.SamplesCount(); j++) {
      switch (base_) {
        case LOG_BASE_E:
          output[j] = logf(input[i]);
          break;
        case LOG_BASE_2:
          output[j] = log2f(input[i]);
          break;
        case LOG_BASE_10:
          output[j] = log10f(input[i]);
          break;
      }
    }
  }
}

REGISTER_TRANSFORM(Log);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
