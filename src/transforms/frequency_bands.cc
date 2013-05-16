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
#include "src/transforms/fork.h"
#include "src/primitives/memory.h"

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

void FrequencyBands::InitializeBuffers(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), inputFormat_->Size());
}

void FrequencyBands::Do(
    const BuffersBase<Formats::WindowF>& in,
    BuffersBase<Formats::WindowF>* out) const noexcept {
  if (in.Size() % bandsNumber_ != 0) {
    WRN("Warning: the number of windows is not a multiple "
        "of bands number. The remainder is left untouched.");
  }

  std::vector<int> bands;
  if (bandsConfig_ == "") {
    for (int i = 0; i < bandsNumber_; i++) {
      bands.push_back(inputFormat_->Size() * (i + 1) / bandsNumber_);
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
        bands.push_back(inputFormat_->Size());
        break;
      }
      bands.push_back((freq * 2 * inputFormat_->Size()) /
                      inputFormat_->SamplingRate());
    }
  }

  for (size_t i = 0; i < in.Size(); i += bandsNumber_) {
    for (int j = 0; j < bandsNumber_; j++) {
      if (j > 0) {
        memsetf((*out)[i + j]->Data.get(),
                bands[j - 1], 0.f);
      }
      if (j < bandsNumber_ - 1) {
        memsetf((*out)[i + j]->Data.get() + bands[j],
                inputFormat_->Size() - bands[j], 0.f);
      }
      if ((*out)[i + j]->Data.get() != in[i + j]->Data.get()) {
        int offset = j > 0? bands[j - 1] : 0;
        memcpy((*out)[i + j]->Data.get() + offset,
               in[i + j]->Data.get() + offset,
               (bands[j] - offset) * sizeof(float));
      }
    }
  }
}

REGISTER_TRANSFORM(FrequencyBands);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
