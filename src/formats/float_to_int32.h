/*! @file intF_to_int32.h
 *  @brief float to int32 converter.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FORMATS_FLOAT_TO_INT32_H_
#define SRC_FORMATS_FLOAT_TO_INT32_H_

#include "src/formats/raw_format.h"
#include "src/formats/raw_format_converter_base.h"
#include "src/formats/window_format.h"
#include "src/formats/window_format_converter_base.h"

namespace SoundFeatureExtraction {
namespace Formats {

class FloatToInt32Raw
    : public RawFormatConverterBase<RawFormatF, RawFormat32> {
 protected:
  virtual void Do(const BuffersBase<RawF>& in,
                          BuffersBase<Raw32> *out) const noexcept;
};

class FloatToInt32Window
    : public WindowFormatConverterBase<WindowFormatF, WindowFormat32> {
 protected:
  virtual void Do(const BuffersBase<WindowF>& in,
                          BuffersBase<Window32> *out) const noexcept;
};

}  // namespace Formats
}  // namespace SoundFeatureExtraction
#endif  // SRC_FORMATS_FLOAT_TO_INT32_H_
