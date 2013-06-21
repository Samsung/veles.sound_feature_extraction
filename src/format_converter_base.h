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

#ifndef SRC_FORMAT_CONVERTER_BASE_H_
#define SRC_FORMAT_CONVERTER_BASE_H_

#include "src/format_converter.h"
#include "src/omp_transform_base.h"

namespace SoundFeatureExtraction {

template <typename FIN, typename FOUT>
class FormatConverterBase
    : public virtual FormatConverter,
      public virtual OmpTransformBase<FIN, FOUT> {
 public:
  virtual void SetInputFormat(const std::shared_ptr<BufferFormat>& format)
      override final {
    TransformBase<FIN, FOUT>::SetInputFormat(format);
  }

  virtual const std::unordered_map<std::string, ParameterTraits>&
  SupportedParameters() const noexcept override final {
    return FormatConverter::SupportedParameters();
  }

  virtual const std::unordered_map<std::string, std::string>&
  GetParameters() const noexcept override final {
    return FormatConverter::GetParameters();
  }

  virtual void SetParameters(
      const std::unordered_map<std::string, std::string>&) override final {
  }

  virtual void Initialize() const noexcept override final {
  }

 protected:
  virtual void OnInputFormatChanged() = 0;
};

}  // namespace SoundFeatureExtraction


#endif  // SRC_FORMAT_CONVERTER_BASE_H_
