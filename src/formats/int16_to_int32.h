/*! @file int16_to_int32.h
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

#ifndef INT16_TO_INT32_H_
#define INT16_TO_INT32_H_

#include "src/formats/raw_format.h"
#include "src/formats/raw_format_converter_base.h"
#include "src/formats/window_format.h"
#include "src/formats/window_format_converter_base.h"

namespace SpeechFeatureExtraction {
namespace Formats {

class Int16ToInt32Raw
    : public RawFormatConverterBase<RawFormat16, RawFormat32> {
 protected:
  virtual void TypeSafeDo(const BuffersBase<Raw16>& in,
                          BuffersBase<Raw32> *out) const noexcept;
};

class Int16ToInt32Window
    : public WindowFormatConverterBase<WindowFormat16, WindowFormat32> {
 protected:
  virtual void TypeSafeDo(const BuffersBase<Window16>& in,
                          BuffersBase<Window32> *out) const noexcept;
};

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_INT16_TO_INT32_H_
