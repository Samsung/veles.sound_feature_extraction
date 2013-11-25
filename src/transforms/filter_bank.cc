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
#include <sstream>
#include <simd/arithmetic-inl.h>
#include "src/transforms/filter_base.h"
#include "src/primitives/energy.h"
#include "src/make_unique.h"

namespace sound_feature_extraction {
namespace transforms {

ScaleType Parse(const std::string& value, identity<ScaleType>) {
  static const std::unordered_map<std::string, ScaleType> map = {
    { internal::kScaleTypeLinearStr, ScaleType::kLinear },
    { internal::kScaleTypeMelStr, ScaleType::kMel },
    { internal::kScaleTypeBarkStr, ScaleType::kBark }
  };
  auto tit = map.find(value);
  if (tit == map.end()) {
    throw InvalidParameterValueException();
  }
  return tit->second;
}

constexpr ScaleType FilterBank::kDefaultScale;

FilterBank::FilterBank()
    : type_(kDefaultScale),
      number_(kDefaultNumber),
      frequency_min_(kDefaultMinFrequency),
      frequency_max_(kDefaultMaxFrequency) {
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

ALWAYS_VALID_TP(FilterBank, squared)
ALWAYS_VALID_TP(FilterBank, debug)

const std::vector<FilterBank::Filter>& FilterBank::filter_bank() const {
  return filter_bank_;
}

float FilterBank::LinearToScale(ScaleType type, float freq) {
  switch (type) {
    case ScaleType::kLinear:
      return freq;
    case ScaleType::kMel:
      return 1127.0f * logf(1 + freq / 700.0f);
    case ScaleType::kBark:
      // see http://depository.bas-net.by/EDNI/Periodicals/Articles/Details.aspx?Key_Journal=32&Id=681
      return 8.96f * logf(0.978f +
                          5.0f * logf(0.994f +
                                      powf((freq + 75.4f) / 2173.0f, 1.347f)));
  }
  return 0.0f;
}

float FilterBank::ScaleToLinear(ScaleType type, float value) {
  switch (type) {
    case ScaleType::kLinear:
      return value;
    case ScaleType::kMel:
      return 700.0f * (expf(value / 1127.0f) - 1);
    case ScaleType::kBark:
      // see http://depository.bas-net.by/EDNI/Periodicals/Articles/Details.aspx?Key_Journal=32&Id=681
      return 2173.0f * powf(expf((expf(value / 8.96f) - 0.978f) / 5.0f) -
                            0.994f, 1.0f / 1.347f) - 75.4f;
  }
  return 0.0f;
}

void FilterBank::CalcTriangularFilter(float center, float halfWidth,
                                      Filter* out) const {
  float left_freq = ScaleToLinear(type_, center - halfWidth);
  float center_freq = ScaleToLinear(type_, center);
  float right_freq = ScaleToLinear(type_, center + halfWidth);

  // The number of frequency points
  const int N = input_format_->Size();
  // Frequency resolution
  const float df = input_format_->SamplingRate() / (2 * N);

  const int left_index = ceilf(left_freq / df);
  const float center_index = center_freq / df;
  const int right_index = floorf(right_freq / df);
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
  out->begin = left_index;
  out->end = right_index;
  for (int i = left_index; i <= right_index; i++) {
    float value = 1.0f;
    float dist = (center - LinearToScale(type_, i * df)) / halfWidth;
    if (i <= center_index) {
      // Left slope
      value -= dist;
    } else {
      // Right slope
      value += dist;
    }
    out->data[i - left_index] = value;
  }
  out->data[roundf(center_index) - left_index] = 1.f;
}

void FilterBank::Initialize() const {
  filter_bank_.resize(number_);
  int count = threads_number();
  buffers_.resize(count);
  for (int i = 0; i < count; i++) {
    buffers_[i].data = std::uniquify(mallocf(input_format_->Size()),
                                     std::free);
  }

  float scaleMin = LinearToScale(type_, frequency_min_);
  float scaleMax = LinearToScale(type_, frequency_max_);
  float dsc = (scaleMax - scaleMin) / (number_ + 1);

  for (int i = 0; i < number_; i++) {
    filter_bank_[i].data = std::uniquify(mallocf(input_format_->Size()),
                                         std::free);
    CalcTriangularFilter(scaleMin + dsc * (i + 1), dsc, &filter_bank_[i]);
  }

  if (squared_) {
    for (int i = 0; i < number_; i++) {
      real_multiply_array(filter_bank_[i].data.get(),
                          filter_bank_[i].data.get(),
                          filter_bank_[i].end - filter_bank_[i].begin + 1,
                          filter_bank_[i].data.get());
    }
  }
  if (debug_) {
    std::stringstream ss;
    for (int i = 0; i < number_; i++) {
      ss << "Filter " << (i + 1) << ":\n";
      for (int j = 0; j < static_cast<int>(input_format_->Size()); j++) {
        auto val = std::to_string(0.f);
        if (j >= filter_bank_[i].begin && j <= filter_bank_[i].end) {
          val = std::to_string(filter_bank_[i].data[j - filter_bank_[i].begin]);
        }
        if (val.size() < 10) {
          val = std::string(10 - val.size(), ' ') + val;
        }
        ss << val << (j % 10 == 9? "\n" : "");
      }
      ss << "\n\n";
    }
    DBG("\n%s", ss.str().c_str());
  }
}

size_t FilterBank::OnInputFormatChanged(size_t buffersCount) {
  size_t start = frequency_min_ * 2 * input_format_->Size() /
      input_format_->SamplingRate();
  size_t finish = frequency_max_ * 2 * input_format_->Size() /
      input_format_->SamplingRate();
  size_t length = finish - start;
  if (length > input_format_->Size() || length <= 0) {
    throw InvalidFrequencyRangeException(frequency_min_, frequency_max_);
  }
  output_format_->SetSize(number_);
  return buffersCount;
}

void FilterBank::Do(const float* in, float* out) const noexcept {
  bool executed = false;
  while (!executed) {
    for (auto& buf : buffers_) {
      if (buf.mutex.try_lock()) {
        for (int i = 0; i < number_; i++) {
          size_t length = filter_bank_[i].end - filter_bank_[i].begin + 1;
          real_multiply_array(in + filter_bank_[i].begin,
                              filter_bank_[i].data.get(),
                              length, buf.data.get());
          out[i] = calculate_energy(use_simd(), false, buf.data.get(), length);
        }
        buf.mutex.unlock();
        executed = true;
        break;
      }
    }
  }
}

RTP(FilterBank, type)
RTP(FilterBank, number)
RTP(FilterBank, frequency_min)
RTP(FilterBank, frequency_max)
RTP(FilterBank, squared)
RTP(FilterBank, debug)
REGISTER_TRANSFORM(FilterBank);

}  // namespace transforms
}  // namespace sound_feature_extraction
