/*! @file int32_to_float.h
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

#ifndef SRC_FORMATS_INT32_TO_FLOAT_H_
#define SRC_FORMATS_INT32_TO_FLOAT_H_

#include "src/formats/raw_format.h"
#include "src/formats/raw_format_converter_base.h"
#include "src/formats/window_format.h"
#include "src/formats/window_format_converter_base.h"

namespace SpeechFeatureExtraction {
namespace Formats {

class Int32ToFloatRaw
    : public RawFormatConverterBase<RawFormat32, RawFormatF> {
 protected:
  virtual void Do(const BuffersBase<Raw32>& in,
                          BuffersBase<RawF> *out) const noexcept;
};

class Int32ToFloatWindow
    : public WindowFormatConverterBase<WindowFormat32, WindowFormatF> {
 protected:
  virtual void Do(const BuffersBase<Window32>& in,
                          BuffersBase<WindowF>* out) const noexcept;
};

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // SRC_FORMATS_INT32_TO_FLOAT_H_
