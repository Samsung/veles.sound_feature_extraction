/*! @file raw_format_converter_base.h
 *  @brief New file description.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef RAW_FORMAT_CONVERTER_BASE_H_
#define RAW_FORMAT_CONVERTER_BASE_H_

#include "src/format_converter_base.h"

namespace SpeechFeatureExtraction {

template <typename FIN, typename FOUT>
class RawFormatConverterBase : public FormatConverterBase<FIN, FOUT> {
 protected:
  virtual ~RawFormatConverterBase() {}

  virtual void TypeSafeInitializeBuffers(
      const BuffersBase<typename FIN::BufferType>& in,
      BuffersBase<typename FOUT::BufferType>* buffers) const noexcept {
    buffers->Initialize(in.Size(),
                        FormatConverterBase<FIN, FOUT>::inputFormat_.Size()
  #ifdef __AVX__
                        , in[0]->AlignmentOffset()
  #endif
    );
  }
};

}  // namespace SpeechFeatureExtraction

#endif  // INCLUDE_RAW_FORMAT_CONVERTER_BASE_H_
