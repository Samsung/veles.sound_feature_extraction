/*! @file array_format_converter_base.h
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

#ifndef SRC_FORMATS_ARRAY_FORMAT_CONVERTER_BASE_H_
#define SRC_FORMATS_ARRAY_FORMAT_CONVERTER_BASE_H_

#include "src/format_converter_base.h"

namespace sound_feature_extraction {

template <typename FIN, typename FOUT>
class ArrayFormatConverterBase : public FormatConverterBase<FIN, FOUT> {
 protected:
  virtual size_t OnInputFormatChanged(size_t buffersCount) override final {
    auto& outputFormat = FormatConverterBase<FIN, FOUT>::output_format_;
    auto& inputFormat = FormatConverterBase<FIN, FOUT>::input_format_;
    outputFormat->SetSize(inputFormat->Size());
    return buffersCount;
  }
};

}  // namespace sound_feature_extraction

#endif  // SRC_FORMATS_ARRAY_FORMAT_CONVERTER_BASE_H_
