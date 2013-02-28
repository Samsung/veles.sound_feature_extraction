/*! @file filter_bank.cc
 *  @brief Psychoacoustic scale conversion.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/filter_bank.h"
#include <math.h>
#include <string>
#include "src/formats/format_limits.h"
#include "src/primitives/arithmetic-inl.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

FilterBank::FilterBank()
  : UniformFormatTransform(SupportedParameters()),
    type_(SCALE_TYPE_MEL),
    length_(DEFAULT_FB_LENGTH),
    minFreq_(DEFAULT_FB_MIN_FREQ),
    maxFreq_(DEFAULT_FB_MAX_FREQ) {
}

bool FilterBank::HasInverse() const noexcept {
  return true;
}

void FilterBank::SetParameter(const std::string& name,
                                 const std::string& value) {
  if (name == "number") {
    auto pv = Parse<size_t>(name, value);
    if (pv > 2048) {
      throw new InvalidParameterValueException(name, value, Name());
    }
    length_ = pv;
  } else if (name == "frequency_min") {
    auto pv = Parse<int>(name, value);
    if (pv < 10 || pv >= maxFreq_) {
      throw new InvalidParameterValueException(name, value, Name());
    }
    minFreq_ = pv;
  } else if (name == "frequency_min") {
    auto pv = Parse<int>(name, value);
    if (pv < 10 || pv <= minFreq_) {
      throw new InvalidParameterValueException(name, value, Name());
    }
    maxFreq_ = pv;
  }
}

float FilterBank::LinearToScale(ScaleType type, float freq) {
  switch (type) {
    case SCALE_TYPE_MEL:
      return 1127.0f * logf(1 + freq / 700.0f);
      break;
    case SCALE_TYPE_BARK:
      // see http://depository.bas-net.by/EDNI/Periodicals/Articles/Details.aspx?Key_Journal=32&Id=681
      return 8.96f * logf(0.978f +
                          5.0f * logf(0.994f +
                                      powf((freq + 75.4f) / 2173.0f, 1.347f)));
      break;
  }
  return 0.0f;
}

float FilterBank::ScaleToLinear(ScaleType type, float value) {
  switch (type) {
    case SCALE_TYPE_MEL:
      return 700.0f * (expf(value / 1127.0f) - 1);
      break;
    case SCALE_TYPE_BARK:
      // see http://depository.bas-net.by/EDNI/Periodicals/Articles/Details.aspx?Key_Journal=32&Id=681
      return 2173.0f * powf(expf((expf(value / 8.96f) - 0.978f) / 5.0f) -
                            0.994f, 1.0f / 1.347f) - 75.4f;
      break;
  }
  return 0.0f;
}

void FilterBank::AddTriangularFilter(float center, float halfWidth) const {
  float leftFreq = ScaleToLinear(type_, center - halfWidth);
  float centerFreq = ScaleToLinear(type_, center);
  float rightFreq = ScaleToLinear(type_, center + halfWidth);

  // The number of frequency points
  int N = inputFormat_.Size() / 2;
  // Frequency resolution
  float df = inputFormat_.SamplingRate() / (2 * N);

  int leftIndex = ceilf(leftFreq / df);
  int centerIndex = ceilf(centerFreq / df);
  int rightIndex = ceilf(rightFreq / df);
/*
          /|\
         / | \
        /  |  \
       /   |   \
      /    |    \
     /     |     \
    /      |      \
 ---------------------
 c - hw    c     c + hw

  Linear triangle is in the scale space, we convert it to the linear frequency
  space. Thus, the triangle becomes nonlinear, curvy:

       /\
       | ----
      /      \
      |       ---
      |          \
     /            \
     |             --
     |               \
    /                 |
------------------------
  left* center*    right*
*/
  for (int i = leftIndex; i < rightIndex; i++) {
    float ratio;
    if (i <= centerIndex) {
      // Left slope
      ratio = (LinearToScale(type_, i * df) - center + halfWidth) /
              halfWidth;
    } else {
      // Right slope
      ratio = 1.0f - (LinearToScale(type_, i * df) - center) / halfWidth;
    }
    filterBank_.get()[i] += ratio;
  }
}

void FilterBank::Initialize() const noexcept {
  filterBank_ = std::shared_ptr<float>(
      mallocf(inputFormat_.Size()),
      [](float* ptr) {
        free(ptr);
      });
  memsetf(filterBank_.get(), inputFormat_.Size(), 0.0f);

  float scaleMin = LinearToScale(type_, minFreq_);
  float scaleMax = LinearToScale(type_, maxFreq_);
  float dsc = (scaleMax - scaleMin) / (length_ - 1);

  for (size_t i = 0; i < length_; i++) {
    AddTriangularFilter(scaleMin + dsc * i, dsc);
  }
}

void FilterBank::TypeSafeInitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_.Size());
}

void FilterBank::TypeSafeDo(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF> *out) const noexcept {
  auto filter = filterBank_.get();
  int N = inputFormat_.Size();
  for (size_t i = 0; i < in.Size(); i++) {
    auto input = in[i]->Data.get();
    auto output = (*out)[i]->Data.get();
#ifdef __AVX__
    for (int i = 0; i < N - 7; i += 8) {
      real_multiply(input + i, filter + i, output + i);
    }
    for (int i = (N >> 3) << 3; i < N; i++) {
      output[i] = input[i] * filter[i];
    }
#elif defined(__ARM_NEON__)
    for (int i = 0; i < N - 3; i += 4) {
      real_multiply(input + i, filter + i, output + i);
    }
    for (size_t i = (N >> 2) << 2; i < N; i++) {
      output[i] = input[i] * filter[i];
    }
#else
    for (int i = 0; i < N; i++) {
      output[i] = input[i] * filter[i];
    }
#endif
  }
}

REGISTER_TRANSFORM(FilterBank);

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
