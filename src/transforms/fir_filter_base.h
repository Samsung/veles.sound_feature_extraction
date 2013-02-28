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

#ifndef SRC_TRANSFORMS_FIR_FILTER_BASE_H_
#define SRC_TRANSFORMS_FIR_FILTER_BASE_H_

#include <string>
#include <vector>
#include "src/uniform_format_transform.h"
#include "src/formats/format_limits.h"
#include "src/formats/raw_format.h"
#include "src/primitives/window.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

class FirFilterBase
    : public UniformFormatTransform<Formats::RawFormat16> {
 public:
  FirFilterBase(const std::unordered_map<std::string, ParameterTraits>&
                supportedParameters) noexcept;

  virtual void Initialize() const noexcept;

 protected:
  int length_;

  virtual void SetFilterParameter(const std::string& name,
                                  const std::string& value) = 0;

  virtual void CalculateFilter(float* filter) const noexcept = 0;

  virtual void SetParameter(const std::string& name, const std::string& value);

  virtual void TypeSafeInitializeBuffers(
      const BuffersBase<Formats::Raw16>& in,
      BuffersBase<Formats::Raw16>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Formats::Raw16>& in,
                          BuffersBase<Formats::Raw16> *out) const noexcept;

 private:
  WindowType windowType_;
  mutable std::vector<float> filter_;
  mutable std::vector<float> dataBuffer_;
};

#define FIR_FILTER_PARAMETERS(init) TRANSFORM_PARAMETERS( \
  FORWARD_MACROS( \
      _TP_("length", "Window length in samples", \
           std::to_string(DEFAULT_FILTER_LENGTH)) \
      _TP_("window", "Type of the window. Supported values are " \
           "\"rectangular\" and \"hamming\".", "hamming") \
      init) \
)

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORMS_FIR_FILTER_BASE_H_
