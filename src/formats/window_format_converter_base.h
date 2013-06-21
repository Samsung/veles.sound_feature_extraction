/*! @file window_format_converter_base.h
 *  @brief WindowFormatConverterBase class definition.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FORMATS_WINDOW_FORMAT_CONVERTER_BASE_H_
#define SRC_FORMATS_WINDOW_FORMAT_CONVERTER_BASE_H_

#include "src/format_converter_base.h"

namespace SoundFeatureExtraction {

template <typename FIN, typename FOUT>
class WindowFormatConverterBase : public FormatConverterBase<FIN, FOUT> {
 protected:
  virtual void OnInputFormatChanged() override final {
    auto outputFormat = FormatConverterBase<FIN, FOUT>::outputFormat_;
    auto inputFormat = FormatConverterBase<FIN, FOUT>::inputFormat_;
    outputFormat->SetAllocatedSize(inputFormat->AllocatedSize());
    outputFormat->SetDuration(inputFormat->Duration());
    outputFormat->SetSize(inputFormat->Size());
    outputFormat->SetParentRawSize(inputFormat->ParentRawSize());
  }

  virtual void InitializeBuffers(
      const BuffersBase<typename FIN::BufferType>& in,
      BuffersBase<typename FOUT::BufferType>* buffers)
      const noexcept override final {
      buffers->Initialize(
          in.Size(),
          FormatConverterBase<FIN, FOUT>::inputFormat_->AllocatedSize());
  }
};

}  // namespace SoundFeatureExtraction

#endif  // SRC_FORMATS_WINDOW_FORMAT_CONVERTER_BASE_H_
