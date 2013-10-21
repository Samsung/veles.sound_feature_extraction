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

namespace SoundFeatureExtraction {
namespace Transforms {

using Primitives::WaveletFilterBank;

const std::vector<int> DWPT::kDefaultTreeFingerprint {
  3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  6, 6, 6, 6, 6, 6, 6, 6
};

const std::string DWPT::kDefaultWaveletType = "daub";
const WaveletType DWPT::kDefaultWaveletTypeEnum = WAVELET_TYPE_DAUBECHIES;
const int DWPT::kDefaultWaveletOrder = 8;

DWPT::DWPT()
  : treeFingerprint_(kDefaultTreeFingerprint),
    waveletType_(kDefaultWaveletTypeEnum),
    waveletOrder_(kDefaultWaveletOrder) {
  RegisterSetter("tree", [&](const std::string& value) {
    treeFingerprint_ = WaveletFilterBank::ParseDescription(value);
    return true;
  });
  RegisterSetter("type", [&](const std::string& value) {
    waveletType_ = WaveletFilterBank::ParseWaveletType(value);
    return true;
  });
  RegisterSetter("order", [&](const std::string& value) {
    int order = Parse<int>("order", value);
    WaveletFilterBank::ValidateOrder(waveletType_, order);
    waveletOrder_ = order;
    return true;
  });
}

size_t DWPT::OnFormatChanged(size_t buffersCount) {
  WaveletFilterBank::ValidateLength(treeFingerprint_,
                                    inputFormat_->Size());
  return buffersCount;
}

void DWPT::Initialize() const {
  filterBank_ = std::make_unique<WaveletFilterBank>(
      waveletType_, waveletOrder_, treeFingerprint_);
}

void DWPT::Do(const float* in,
              float* out) const noexcept {
  assert(filterBank_ != nullptr && "Initialize() was not called");
  filterBank_->Apply(in, inputFormat_->Size(), out);
}

void DWPT::DoInverse(const float* in UNUSED,
                     float* out UNUSED) const noexcept {
  assert("Not implemented yet");
}

REGISTER_TRANSFORM(DWPT);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
