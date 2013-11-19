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
#include <cmath>
#include <simd/arithmetic-inl.h>
#include "src/transforms/filter_base.h"

namespace sound_feature_extraction {
namespace transforms {

ScaleType Parse(const std::string& value, identity<ScaleType>) {
  static const std::unordered_map<std::string, ScaleType> map = {
    { internal::kScaleTypeLinearStr, kScaleTypeLinear },
    { internal::kScaleTypeMelStr, kScaleTypeMel },
    { internal::kScaleTypeBarkStr, kScaleTypeBark }
  };
  auto tit = map.find(value);
  if (tit == map.end()) {
    throw InvalidParameterValueException();
  }
  return tit->second;
}

RTP(FilterBank, type)
RTP(FilterBank, number)
RTP(FilterBank, frequency_min)
RTP(FilterBank, frequency_max)

FilterBank::FilterBank()
    : type_(kDefaultScale),
      number_(kDefaultNumber),
      frequency_min_(kDefaultMinFrequency),
      frequency_max_(kDefaultMaxFrequency),
      filter_bank_(nullptr, free) {
}

ALWAYS_VALID_TP(FilterBank, type)

bool FilterBank::validate_number(const int& value) noexcept {
  return value <= 2048;
}

bool FilterBank::validate_frequency_min(const int& value) noexcept {
  return FilterBase<std::nullptr_t>::ValidateFrequency(value);
}

bool FilterBank::validate_frequency_max(const int& value) noexcept {
  return FilterBase<std::nullptr_t>::ValidateFrequency(value);
}

const FloatPtr& FilterBank::filter_bank() const {
  return filter_bank_;
}

float FilterBank::LinearToScale(ScaleType type, float freq) {
  switch (type) {
    case kScaleTypeLinear:
      return freq;
    case kScaleTypeMel:
      return 1127.0f * logf(1 + freq / 700.0f);
    case kScaleTypeBark:
      // see http://depository.bas-net.by/EDNI/Periodicals/Articles/Details.aspx?Key_Journal=32&Id=681
      return 8.96f * logf(0.978f +
                          5.0f * logf(0.994f +
                                      powf((freq + 75.4f) / 2173.0f, 1.347f)));
  }
  return 0.0f;
}

float FilterBank::ScaleToLinear(ScaleType type, float value) {
  switch (type) {
    case kScaleTypeLinear:
      return value;
    case kScaleTypeMel:
      return 700.0f * (expf(value / 1127.0f) - 1);
    case kScaleTypeBark:
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
  int N = input_format_->Size() / 2;
  // Frequency resolution
  float df = input_format_->SamplingRate() / (2 * N);

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
    filter_bank_.get()[i] += ratio;
  }
}

void FilterBank::Initialize() const {
  filter_bank_ = std::unique_ptr<float, void(*)(void*)>(
      mallocf(input_format_->Size()), std::free);
  memsetf(filter_bank_.get(), 0.f, input_format_->Size());

  float scaleMin = LinearToScale(type_, frequency_min_);
  float scaleMax = LinearToScale(type_, frequency_max_);
  float dsc = (scaleMax - scaleMin) / (number_ - 1);

  for (int i = 0; i < number_; i++) {
    AddTriangularFilter(scaleMin + dsc * i, dsc);
  }

  // Avoid zeros in filter since taking a logarithm from 0 is undefined.
  auto filter = filter_bank_.get();
  for (size_t i = 0; i < input_format_->Size(); i++) {
    if (filter[i] == 0.f) {
      filter[i] = 0.0001f;
    }
    assert(filter[i] >= 0.f);
  }
}

void FilterBank::Do(const float* in, float* out) const noexcept {
  auto filter = filter_bank_.get();
  int length = input_format_->Size();
#ifdef SIMD
  for (int i = 0; i < length - FLOAT_STEP + 1; i += FLOAT_STEP) {
    real_multiply(in + i, filter + i, out + i);
  }
  for (int i = (length >> FLOAT_STEP_LOG2) << FLOAT_STEP_LOG2;
      i < length; i++) {
    out[i] = in[i] * filter[i];
  }
#else
  for (int i = 0; i < length; i++) {
    out[i] = in[i] * filter[i];
  }
#endif
}

REGISTER_TRANSFORM(FilterBank);

}  // namespace transforms
}  // namespace sound_feature_extraction
