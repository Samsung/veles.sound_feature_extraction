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
  SingleFrequencyFilter() noexcept : frequency_(B::kMinFilterFrequency) {
    this->RegisterSetter(kFrequencyParameterName, [&](const std::string& value) {
      int pv = this->template Parse<int>(kFrequencyParameterName, value);
      if (pv < B::kMinFilterFrequency || pv > B::kMaxFilterFrequency) {
        return false;
      }
      frequency_ = pv;
      return true;
    });
  }

  int frequency() const {
    return frequency_;
  }

  void set_frequency(int value) {
    this->SetParameter(kFrequencyParameterName, std::to_string(value));
  }

  static constexpr const char* kFrequencyParameterName = "frequency";

 private:
  int frequency_;
};

template <class B>
constexpr const char* SingleFrequencyFilter<B>::kFrequencyParameterName;

}  // namespace sound_feature_extraction
}  // namespace transforms

#endif  // SRC_TRANSFORMS_SINGLE_FREQUENCY_FILTER_H_
