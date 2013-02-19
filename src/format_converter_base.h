/*! @file format_converter_base.h
 *  @brief Base class of all converters between formats.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef FORMAT_CONVERTER_BASE_H_
#define FORMAT_CONVERTER_BASE_H_

#include "src/format_converter.h"
#include "src/transform_base.h"

namespace SpeechFeatureExtraction {

template <typename FIN, typename FOUT>
class FormatConverterBase
    : public FormatConverter, public TransformBase<FIN, FOUT> {
 public:
  FormatConverterBase() : TransformBase<FIN, FOUT>(SupportedParameters()) {
  }

  virtual void SetInputFormat(const BufferFormat& format) {
    FormatConverter::SetInputFormat(format);
  }

  virtual const std::unordered_map<std::string, ParameterTraits>&
  SupportedParameters() const noexcept {
    return FormatConverter::SupportedParameters();
  }

  virtual const std::unordered_map<std::string, std::string>&
  CurrentParameters() const noexcept {
    return FormatConverter::CurrentParameters();
  }

  virtual void SetParameters(
      const std::unordered_map<std::string, std::string>&) {
  }

  virtual void Initialize() const noexcept {
  }
};

}  // namespace SpeechFeatureExtraction


#endif  // INCLUDE_FORMAT_CONVERTER_BASE_H_
