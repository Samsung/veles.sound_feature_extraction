/*! @file dwpt.cc
 *  @brief Discrete wavelet packet transform.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/dwpt.h"
#include "src/make_unique.h"

namespace sound_feature_extraction {
namespace transforms {

using primitives::WaveletFilterBank;

DWPT::DWPT()
    : tree_(kDefaultTreeFingerprint()),
      type_(kDefaultWaveletType),
      order_(kDefaultWaveletOrder) {
}

bool DWPT::validate_tree(const TreeFingerprint& value) noexcept {
  try {
    WaveletFilterBank::ValidateDescription(value);
  }
  catch(const primitives::WaveletTreeInvalidDescriptionException&) {
    return false;
  }
  return true;
}

ALWAYS_VALID_TP(DWPT, type)

bool DWPT::validate_order(const int& value) noexcept {
  return value >= 2;
}

size_t DWPT::OnFormatChanged(size_t buffersCount) {
  WaveletFilterBank::ValidateLength(tree_,
                                    input_format_->Size());
  return buffersCount;
}

void DWPT::Initialize() const {
  filter_bank_ = std::make_unique<WaveletFilterBank>(
      type_, order_, tree_);
  WaveletFilterBank::ValidateWavelet(type_, order_);
}

void DWPT::Do(const float* in,
              float* out) const noexcept {
  assert(filter_bank_ != nullptr && "Initialize() was not called");
  filter_bank_->Apply(in, input_format_->Size(), out);
}

RTP(DWPT, tree)
RTP(DWPT, type)
RTP(DWPT, order)
REGISTER_TRANSFORM(DWPT);

}  // namespace transforms
}  // namespace sound_feature_extraction
