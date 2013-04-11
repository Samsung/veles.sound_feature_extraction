/*! @file raw_format_converter_base.h
 *  @brief RawFormatConverterBase class definition.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FORMATS_RAW_FORMAT_CONVERTER_BASE_H_
#define SRC_FORMATS_RAW_FORMAT_CONVERTER_BASE_H_

#include "src/format_converter_base.h"

namespace SoundFeatureExtraction {

template <typename FIN, typename FOUT>
class RawFormatConverterBase : public FormatConverterBase<FIN, FOUT> {
 protected:
  virtual ~RawFormatConverterBase() {}

  virtual void OnInputFormatChanged() {
    auto outputFormat = FormatConverterBase<FIN, FOUT>::outputFormat_;
    auto inputFormat = FormatConverterBase<FIN, FOUT>::inputFormat_;
    outputFormat->SetSamplingRate(inputFormat->SamplingRate());
    outputFormat->SetSize(inputFormat->Size());
  }

  virtual void InitializeBuffers(
      const BuffersBase<typename FIN::BufferType>& in,
      BuffersBase<typename FOUT::BufferType>* buffers) const noexcept {
    buffers->Initialize(in.Size(),
                        FormatConverterBase<FIN, FOUT>::inputFormat_->Size(),
                        in[0]->AlignmentOffset());
  }
};

}  // namespace SoundFeatureExtraction

#endif  // SRC_FORMATS_RAW_FORMAT_CONVERTER_BASE_H_
