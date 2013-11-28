/*! @file reorder.cc
 *  @brief Reorders the values in the array.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/reorder.h"

namespace sound_feature_extraction {
namespace transforms {

ReorderAlgorithm Parse(const std::string& value, identity<ReorderAlgorithm>) {
  static const std::unordered_map<std::string, ReorderAlgorithm> map {
    { internal::kReorderAlgorithmChromaStr, ReorderAlgorithm::kChroma }
  };
  auto rait = map.find(value);
  if (rait == map.end()) {
    throw InvalidParameterValueException();
  }
  return rait->second;
}

constexpr ReorderAlgorithm Reorder::kDefaultAlgorithm;

Reorder::Reorder() : algorithm_(kDefaultAlgorithm) {
}

ALWAYS_VALID_TP(Reorder, algorithm)

size_t Reorder::OnFormatChanged(size_t buffersCount) {
  switch (algorithm_) {
    case ReorderAlgorithm::kChroma:
      output_format_->SetSize(
          input_format_->Size() - input_format_->Size() % 12);
      break;
  }
  return buffersCount;
}

void Reorder::Do(const float* in, float* out) const noexcept {
  switch (algorithm_) {
    case ReorderAlgorithm::kChroma:
      DoChroma(in, out);
      break;
  }
}

void Reorder::DoChroma(const float* in, float* out) const noexcept {
  int step = output_format_->Size() / 12;
  for (int i = 0; i < step; i++) {
    for (int j = 0; j < 12; j++) {
      out[j * step + i] = in[i * 12];
    }
  }
}

RTP(Reorder, algorithm)
REGISTER_TRANSFORM(Reorder);

}  // namespace transforms
}  // namespace sound_feature_extraction
