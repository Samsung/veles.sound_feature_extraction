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

namespace SpeechFeatureExtraction {

template <typename FIN, typename FOUT>
class WindowFormatConverterBase : public FormatConverterBase<FIN, FOUT> {
 protected:
  virtual ~WindowFormatConverterBase() {}

  virtual void OnInputFormatChanged() {
    FormatConverterBase<FIN, FOUT>::outputFormat_->SetAllocatedSize(
        FormatConverterBase<FIN, FOUT>::inputFormat_->AllocatedSize());
    FormatConverterBase<FIN, FOUT>::outputFormat_->SetDuration(
        FormatConverterBase<FIN, FOUT>::inputFormat_->Duration());
    FormatConverterBase<FIN, FOUT>::outputFormat_->SetSamplingRate(
        FormatConverterBase<FIN, FOUT>::inputFormat_->SamplingRate());
    FormatConverterBase<FIN, FOUT>::outputFormat_->SetSize(
        FormatConverterBase<FIN, FOUT>::inputFormat_->Size());
  }

  virtual void InitializeBuffers(
      const BuffersBase<typename FIN::BufferType>& in,
      BuffersBase<typename FOUT::BufferType>* buffers) const noexcept {
      buffers->Initialize(
          in.Size(),
          FormatConverterBase<FIN, FOUT>::inputFormat_->SamplesCount());
  }
};

}  // namespace SpeechFeatureExtraction

#endif  // SRC_FORMATS_WINDOW_FORMAT_CONVERTER_BASE_H_
