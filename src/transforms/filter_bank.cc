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
#include "src/formats/format_limits.h"
#include "src/primitives/arithmetic-inl.h"

namespace SoundFeatureExtraction {
namespace Transforms {

const std::unordered_map<std::string, FilterBank::ScaleType>
FilterBank::kScaleTypeMap = {
    { "linear", SCALE_TYPE_LINEAR },
    { "mel", SCALE_TYPE_MEL },
    { "bark", SCALE_TYPE_BARK }
};

const int FilterBank::kDefaultLength = 40;
const int FilterBank::kDefaultMinFrequency = 130;
const int FilterBank::kDefaultMaxFrequency = 6854;

FilterBank::FilterBank()
  : type_(SCALE_TYPE_MEL),
    length_(kDefaultLength),
    minFreq_(kDefaultMinFrequency),
    maxFreq_(kDefaultMaxFrequency),
    filterBank_(nullptr, free) {
  RegisterSetter("number", [&](const std::string& value) {
    auto pv = Parse<size_t>("number", value);
    if (pv > 2048) {
      return false;
    }
    length_ = pv;
    return true;
  });
  RegisterSetter("frequency_min", [&](const std::string& value) {
    auto pv = Parse<int>("frequency_min", value);
    if (pv < 10 || pv >= maxFreq_) {
      return false;
    }
    minFreq_ = pv;
    return true;
  });
  RegisterSetter("frequency_max", [&](const std::string& value) {
    auto pv = Parse<int>("frequency_max", value);
    if (pv < 10 || pv <= minFreq_) {
      return false;
    }
    maxFreq_ = pv;
    return true;
  });
  RegisterSetter("type", [&](const std::string& value) {
    auto tit = kScaleTypeMap.find(value);
    if (tit == kScaleTypeMap.end()) {
      return false;
    }
    type_ = tit->second;
    return true;
  });
}

float FilterBank::LinearToScale(ScaleType type, float freq) {
  switch (type) {
    case SCALE_TYPE_LINEAR:
      return freq;
    case SCALE_TYPE_MEL:
      return 1127.0f * logf(1 + freq / 700.0f);
    case SCALE_TYPE_BARK:
      // see http://depository.bas-net.by/EDNI/Periodicals/Articles/Details.aspx?Key_Journal=32&Id=681
      return 8.96f * logf(0.978f +
                          5.0f * logf(0.994f +
                                      powf((freq + 75.4f) / 2173.0f, 1.347f)));
  }
  return 0.0f;
}

float FilterBank::ScaleToLinear(ScaleType type, float value) {
  switch (type) {
    case SCALE_TYPE_LINEAR:
      return value;
    case SCALE_TYPE_MEL:
      return 700.0f * (expf(value / 1127.0f) - 1);
    case SCALE_TYPE_BARK:
      // see http://depository.bas-net.by/EDNI/Periodicals/Articles/Details.aspx?Key_Journal=32&Id=681
      return 2173.0f * powf(expf((expf(value / 8.96f) - 0.978f) / 5.0f) -
                            0.994f, 1.0f / 1.347f) - 75.4f;
  }
  return 0.0f;
}

void FilterBank::AddTriangularFilter(float center, float halfWidth) const {
  float leftFreq = ScaleToLinear(type_, center - halfWidth);
  float centerFreq = ScaleToLinear(type_, center);
  float rightFreq = ScaleToLinear(type_, center + halfWidth);

  // The number of frequency points
  int N = inputFormat_->Size() / 2;
  // Frequency resolution
  float df = inputFormat_->SamplingRate() / (2 * N);

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

       xxxxxxx
     xxx      xx
    xx         xxx
    x             xx
   xx              xx
   x                 x
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
  filterBank_ = std::unique_ptr<float, void(*)(void*)>(mallocf(inputFormat_->Size()), free);
  memsetf(filterBank_.get(), inputFormat_->Size(), 0.f);

  float scaleMin = LinearToScale(type_, minFreq_);
  float scaleMax = LinearToScale(type_, maxFreq_);
  float dsc = (scaleMax - scaleMin) / (length_ - 1);

  for (size_t i = 0; i < length_; i++) {
    AddTriangularFilter(scaleMin + dsc * i, dsc);
  }

  // Avoid zeros in filter since taking a logarithm from 0 is undefined.
  auto filter = filterBank_.get();
  for (size_t i = 0; i < inputFormat_->Size(); i++) {
    if (filter[i] == 0.f) {
      filter[i] = 0.0001f;
    }
    assert(filter[i] >= 0.f);
  }
}

void FilterBank::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size());
}

void FilterBank::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
  auto filter = filterBank_.get();
  int length = inputFormat_->Size();
  for (size_t i = 0; i < in.Size(); i++) {
    auto input = in[i]->Data.get();
    auto output = (*out)[i]->Data.get();
#ifdef SIMD
    for (int i = 0; i < length - FLOAT_STEP + 1; i += FLOAT_STEP) {
      real_multiply(input + i, filter + i, output + i);
    }
    for (int i = (length >> FLOAT_STEP_LOG2) << FLOAT_STEP_LOG2;
        i < length; i++) {
      output[i] = input[i] * filter[i];
    }
#else
    for (int i = 0; i < length; i++) {
      output[i] = input[i] * filter[i];
    }
#endif
  }
}

REGISTER_TRANSFORM(FilterBank);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
