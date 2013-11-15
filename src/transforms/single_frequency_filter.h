/*! @file single_frequency_filter.h
 *  @brief Low- and highpass filters common features.
 *  @author Vadim Markovtsev <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORMS_SINGLE_FREQUENCY_FILTER_H_
#define SRC_TRANSFORMS_SINGLE_FREQUENCY_FILTER_H_

namespace sound_feature_extraction {
namespace transforms {

template <class B>
class SingleFrequencyFilter : public B {
 public:
  TRANSFORM_PARAMETERS_SUPPORT(SingleFrequencyFilter<B>)

  SingleFrequencyFilter() noexcept : frequency_(B::kMinFilterFrequency) {
  }

  TP(frequency, int, B::kMinFilterFrequency, "Filter's cutoff frequency.")
};

template <class B>
bool SingleFrequencyFilter<B>::validate_frequency(const int& value) noexcept {
  return B::ValidateFrequency(value);
}

template <class B>
RTP(SingleFrequencyFilter<B>, frequency)

}  // namespace sound_feature_extraction
}  // namespace transforms

#endif  // SRC_TRANSFORMS_SINGLE_FREQUENCY_FILTER_H_
