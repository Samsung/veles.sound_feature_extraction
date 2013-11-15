/*! @file subband_energy.cc
 *  @brief Calculate the subband energy.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/subband_energy.h"
#include <cmath>
#ifdef __AVX__
#include <immintrin.h>
#include <simd/avx_extra.h>
#elif defined(__ARM_NEON__)
#include <arm_neon.h>  // NOLINT(build/include_order)
#endif
#include "src/primitives/energy.h"

namespace sound_feature_extraction {
namespace transforms {

using primitives::WaveletFilterBank;

SubbandEnergy::SubbandEnergy()
    : tree_(kDefaultTreeFingerprint()) {
}

bool SubbandEnergy::validate_tree(const TreeFingerprint& value) noexcept {
  try {
    WaveletFilterBank::ValidateDescription(value);
  }
  catch(const primitives::WaveletTreeInvalidDescriptionException&) {
    return false;
  }
  return true;
}

size_t SubbandEnergy::OnFormatChanged(size_t buffersCount) {
  WaveletFilterBank::ValidateLength(tree_,
                                    input_format_->Size());
  output_format_->SetSize(tree_.size());
  return buffersCount;
}

void SubbandEnergy::Initialize() const {
  offsets_.reserve(tree_.size());
  int offset = 0;
  for (auto depth : tree_) {
    offsets_.push_back(offset);
    offset += (input_format_->Size() >> depth);
  }
  offsets_.push_back(offset);
}

void SubbandEnergy::Do(const float* in,
                       float* out) const noexcept {
  for (int i = 0; i < static_cast<int>(offsets_.size()) - 1; i++) {
    out[i] = calculate_energy(use_simd(), in + offsets_[i],
                              offsets_[i + 1] - offsets_[i]);
  }
}

RTP(SubbandEnergy, tree)
REGISTER_TRANSFORM(SubbandEnergy);

}  // namespace transforms
}  // namespace sound_feature_extraction
