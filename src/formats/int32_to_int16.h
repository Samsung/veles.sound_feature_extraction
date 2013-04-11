/*! @file int32_to_int16.h
 *  @brief int32 to int16 converter.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FORMATS_INT32_TO_INT16_H_
#define SRC_FORMATS_INT32_TO_INT16_H_

#include "src/formats/raw_format.h"
#include "src/formats/raw_format_converter_base.h"
#include "src/formats/window_format.h"
#include "src/formats/window_format_converter_base.h"

namespace SoundFeatureExtraction {
namespace Formats {

class Int32ToInt16Raw
    : public RawFormatConverterBase<RawFormat32, RawFormat16> {
 protected:
  virtual void Do(const BuffersBase<Raw32>& in,
                          BuffersBase<Raw16> *out) const noexcept;
};

class Int32ToInt16Window
    : public WindowFormatConverterBase<WindowFormat32, WindowFormat16> {
 protected:
  virtual void Do(const BuffersBase<Window32>& in,
                          BuffersBase<Window16> *out) const noexcept;
};

}  // namespace Formats
}  // namespace SoundFeatureExtraction
#endif  // SRC_FORMATS_INT32_TO_INT16_H_
