/*! @file raw_format_converter_base.h
 *  @brief Base class for all raw frames format converters.
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
  virtual size_t OnInputFormatChanged(size_t buffersCount) override final {
    auto& outputFormat = FormatConverterBase<FIN, FOUT>::outputFormat_;
    auto& inputFormat = FormatConverterBase<FIN, FOUT>::inputFormat_;
    outputFormat->SetSize(inputFormat->Size());
    return buffersCount;
  }
};

}  // namespace SoundFeatureExtraction

#endif  // SRC_FORMATS_RAW_FORMAT_CONVERTER_BASE_H_
