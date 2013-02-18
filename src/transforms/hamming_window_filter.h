/*! @file hamming_window_filter.h
 *  @brief Base class of all filters built on the Hamming window.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef HAMMING_WINDOW_FILTER_H_
#define HAMMING_WINDOW_FILTER_H_

#include "src/transforms/fir_filter_base.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

/// @brief Base class of all filters built on the Hamming window.
class HammingWindowFilter : public FirFilterBase {
 public:
  HammingWindowFilter(
      const std::unordered_map<std::string, ParameterTraits>&
        supportedParameters) noexcept;

 protected:
  virtual float WindowFunction(int index) const noexcept;
};

}  // namespace Transforms
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_HAMMING_WINDOW_FILTER_H_
