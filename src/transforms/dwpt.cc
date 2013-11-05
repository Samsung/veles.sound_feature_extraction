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

using Primitives::WaveletFilterBank;

const std::vector<int> DWPT::kDefaultTreeFingerprint {
  3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  6, 6, 6, 6, 6, 6, 6, 6
};

const std::string DWPT::kDefaultWaveletType = "daub";
const WaveletType DWPT::kDefaultWaveletTypeEnum = WAVELET_TYPE_DAUBECHIES;
const int DWPT::kDefaultWaveletOrder = 8;

DWPT::DWPT()
  : tree_fingerprint_(kDefaultTreeFingerprint),
    wavelet_type_(kDefaultWaveletTypeEnum),
    wavelet_order_(kDefaultWaveletOrder) {
  RegisterSetter("tree", [&](const std::string& value) {
    tree_fingerprint_ = WaveletFilterBank::ParseDescription(value);
    return true;
  });
  RegisterSetter("type", [&](const std::string& value) {
    wavelet_type_ = WaveletFilterBank::ParseWaveletType(value);
    return true;
  });
  RegisterSetter("order", [&](const std::string& value) {
    int order = Parse<int>("order", value);
    WaveletFilterBank::ValidateOrder(wavelet_type_, order);
    wavelet_order_ = order;
    return true;
  });
}

size_t DWPT::OnFormatChanged(size_t buffersCount) {
  WaveletFilterBank::ValidateLength(tree_fingerprint_,
                                    input_format_->Size());
  return buffersCount;
}

void DWPT::Initialize() const {
  filter_bank_ = std::make_unique<WaveletFilterBank>(
      wavelet_type_, wavelet_order_, tree_fingerprint_);
}

void DWPT::Do(const float* in,
              float* out) const noexcept {
  assert(filter_bank_ != nullptr && "Initialize() was not called");
  filter_bank_->Apply(in, input_format_->Size(), out);
}

REGISTER_TRANSFORM(DWPT);

}  // namespace transforms
}  // namespace sound_feature_extraction
