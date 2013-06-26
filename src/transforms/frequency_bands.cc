/*! @file frequency_bands.cc
 *  @brief Cut the frequency bands from a signal.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/frequency_bands.h"
#include <boost/regex.hpp>
#include <simd/memory.h>
#include "src/transforms/fork.h"

namespace SoundFeatureExtraction {
namespace Transforms {

const int FrequencyBands::kDefaultBandsNumber = Fork::kDefaultFactor;
const std::string FrequencyBands::kDefaultBandsConfig = "";

FrequencyBands::FrequencyBands()
    : bandsNumber_(kDefaultBandsNumber),
      bandsConfig_(kDefaultBandsConfig) {
  RegisterSetter("number", [&](const std::string& value) {
    int pv = Parse<int>("number", value);
    if (pv < 1 || pv > 8000) {
      return false;
    }
    bandsNumber_ = pv;
    return true;
  });
  RegisterSetter("bands", [&](const std::string& value) {
    static const boost::regex allRegex("^\\s*(\\d+\\s*(\\s|$))+");
    boost::smatch match;
    if (!boost::regex_match(value, match, allRegex)) {
      return false;
    }
    static const boost::regex bandsRegex("\\s*(\\d+)\\s*");
    static const boost::sregex_token_iterator empty;
    boost::sregex_token_iterator bandsIterator(
          value.begin(), value.end(), bandsRegex, 1);
    assert(bandsIterator != empty);
    int oldFreq = 0;
    while (bandsIterator != empty) {
      int freq = std::stoi(*bandsIterator++);
      if (freq <= oldFreq) {
        return false;
      }
      oldFreq = freq;
    }
    bandsConfig_ = value;
    return true;
  });
}

void FrequencyBands::Initialize() const noexcept {
  bands_.clear();
  if (bandsConfig_ == "") {
    for (int i = 0; i < bandsNumber_; i++) {
      bands_.push_back(inputFormat_->Size() * (i + 1) / bandsNumber_);
    }
  } else {
    static const boost::regex bandsRegex("\\s*(\\d+)\\s*");
    static const boost::sregex_token_iterator empty;
    boost::sregex_token_iterator bandsIterator(
          bandsConfig_.begin(), bandsConfig_.end(), bandsRegex, 1);
    assert(bandsIterator != empty);
    while (bandsIterator != empty) {
      int freq = std::stoi(*bandsIterator++);
      if (freq > inputFormat_->SamplingRate() / 2) {
        WRN("Warning: the bands after %i (defined by sampling "
            "rate %i) will be discarded (first greater band was "
            "%i).\n",
            inputFormat_->SamplingRate() / 2,
            inputFormat_->SamplingRate(),
            freq);
        bands_.push_back(inputFormat_->Size());
        break;
      }
      bands_.push_back((freq * 2 * inputFormat_->Size()) /
                      inputFormat_->SamplingRate());
    }
    if ((size_t)bands_.back() < inputFormat_->Size()) {
      bands_.push_back(inputFormat_->Size());
    }
  }
}

void FrequencyBands::Do(
    const BuffersBase<float*>& in,
    BuffersBase<float*>* out) const noexcept {

  if (in.Count() % bands_.size() != 0) {
    WRN("Warning: the number of windows (%zu) is not a multiple "
        "of bands number (%zu). The remainder is left untouched.",
        in.Count(), bands_.size());
  }

  for (size_t i = 0; i < in.Count(); i += bands_.size()) {
    for (int j = 0; j < (int)bands_.size(); j++) {
      if (j > 0) {
        memsetf((*out)[i + j],
                bands_[j - 1], 0.f);
      }
      if (j < (int)bands_.size() - 1) {
        memsetf((*out)[i + j] + bands_[j],
                inputFormat_->Size() - bands_[j], 0.f);
      }
      if ((*out)[i + j] != in[i + j]) {
        int offset = j > 0? bands_[j - 1] : 0;
        memcpy((*out)[i + j] + offset,
               in[i + j] + offset,
               (bands_[j] - offset) * sizeof(float));
      }
    }
  }
}

REGISTER_TRANSFORM(FrequencyBands);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
