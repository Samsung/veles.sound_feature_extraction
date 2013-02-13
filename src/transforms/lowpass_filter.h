/*! @file lowpass_filter.h
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef LOWPASS_FILTER_H_
#define LOWPASS_FILTER_H_

#include <vector>
#include "src/transform_base.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class LowpassFilter
    : public TransformBase<Formats::RawFormat, Formats::RawFormat> {
 public:
  LowpassFilter();

  TRANSFORM_NAME("Lowpass")

  TRANSFORM_PARAMETERS(
      _TP_("length", "Hamming window length in samples", "150")
      _TP_("frequency", "Cutoff frequency", "1500")
  )

  virtual void Initialize() const noexcept;

 private:
  const std::vector<float> filter_;

  void FillFilter();
  // TODO: http://homepages.which.net/~paul.hills/Circuits/MercurySwitchFilter/FIR.html
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_LOWPASS_FILTER_H_
