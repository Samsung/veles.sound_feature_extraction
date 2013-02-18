/*! @file fir_filter_base.h
 *  @brief Base class for all FIR filters.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef CONVOLUTION_FILTER_BASE_H_
#define CONVOLUTION_FILTER_BASE_H_

#include <vector>
#include "src/transform_base.h"
#include "src/formats/format_limits.h"
#include "src/formats/raw_format.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class FirFilterBase
    : public TransformBase<Formats::RawFormat, Formats::RawFormat> {
 public:
  FirFilterBase(const std::unordered_map<std::string, ParameterTraits>&
                supportedParameters) noexcept;

  virtual void Initialize() const noexcept;

 protected:
  int length_;

  virtual void SetFilterParameter(const std::string& name,
                                  const std::string& value) = 0;

  virtual float WindowFunction(int index) const noexcept = 0;

  virtual void CalculateFilter(float* filter) const noexcept = 0;

  virtual void SetParameter(const std::string& name, const std::string& value);

  virtual void OnInputFormatChanged();

  virtual void TypeSafeInitializeBuffers(const BuffersBase<Formats::Raw>& in,
      BuffersBase<Formats::Raw>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Formats::Raw>& in,
                          BuffersBase<Formats::Raw> *out) const noexcept;

 private:
  mutable std::vector<float> filter_;
  mutable std::vector<float> dataBuffer_;
};

#define FIR_FILTER_PARAMETERS(init) TRANSFORM_PARAMETERS( \
  FORWARD_MACROS(_TP_("length", "Hamming window length in samples", \
                                std::to_string(DEFAULT_FILTER_LENGTH)) \
      init) \
)

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_CONVOLUTION_FILTER_BASE_H_
