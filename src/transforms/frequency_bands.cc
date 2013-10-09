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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <boost/regex.hpp>
#pragma GCC diagnostic pop
#include "src/safe_omp.h"
#include "src/transforms/fork.h"
#include "src/transforms/lowpass_filter.h"
#include "src/transforms/bandpass_filter.h"
#include "src/transforms/highpass_filter.h"

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
  filters_.clear();
  if (bandsConfig_ == "") {
    /*for (int i = 0; i < bandsNumber_; i++) {
      bands_.push_back(inputFormat_->Size() * (i + 1) / bandsNumber_);
    }*/
  } else {
    int last_freq = 0;
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
        last_freq = freq;
        break;
      }
      if (last_freq == 0) {
        auto filter = std::make_shared<LowpassFilter>();
        filter->set_frequency(freq);
        filters_.push_back(filter);
      } else {
        auto filter = std::make_shared<BandpassFilter>();
        filter->set_frequency_low(last_freq);
        filter->set_frequency_high(freq);
        auto ratio = last_freq / inputFormat_->SamplingRate() * 2;
        if (ratio > 0.1f) {
          if (ratio < 0.3f) {
            filter->set_length(128);
          }
          filter->SetParameter("type", "chebyshevII");
        }
        filters_.push_back(filter);
      }
      last_freq = freq;
    }
    // Append high-pass filter
    auto filter = std::make_shared<HighpassFilter>();
    filter->set_frequency(last_freq);
    auto ratio = last_freq / inputFormat_->SamplingRate() * 2;
    if (ratio > 0.1f) {
      if (ratio < 0.3f) {
        filter->set_length(128);
      }
      filter->SetParameter("type", "chebyshevII");
    }
    filters_.push_back(filter);
  }

  for (const auto& filter : filters_) {
    filter->SetInputFormat(inputFormat_, 1);
    filter->Initialize();
  }
}

void FrequencyBands::Do(const BuffersBase<float*>& in,
                        BuffersBase<float*>* out) const noexcept {
  auto rem = in.Count() % filters_.size();
  if (rem != 0) {
    WRN("Warning: the number of windows (%zu) is not a multiple "
        "of bands number (%zu). The remainder is discarded.",
        in.Count(), filters_.size());
  }

#ifdef HAVE_OPENMP
   #pragma omp parallel for num_threads(this->MaxThreadsNumber())
#endif
  for (size_t i = 0; i < in.Count(); i += filters_.size()) {
    for (int j = 0; j < static_cast<int>(filters_.size()); j++) {
      filters_[j]->Do(in[i + j], (*out)[i + j]);
    }
  }
}

REGISTER_TRANSFORM(FrequencyBands);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
