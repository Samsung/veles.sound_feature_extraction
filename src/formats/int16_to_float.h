/*! @file int16_to_float.h
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

#ifndef INT16_TO_FLOAT_H_
#define INT16_TO_FLOAT_H_

#include "src/format_converter_base.h"
#include "src/formats/raw_format.h"
#include "src/formats/window_format.h"

namespace SpeechFeatureExtraction {
namespace Formats {

class Int16ToFloatRaw
    : public FormatConverterBase<RawFormat16, RawFormatF> {
 protected:
  virtual void TypeSafeInitializeBuffers(
      const BuffersBase<Raw16>& in,
      BuffersBase<RawF>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Raw16>& in,
                          BuffersBase<RawF> *out) const noexcept;
};

class Int16ToFloatWindow
    : public FormatConverterBase<WindowFormat16, WindowFormatF> {
 protected:
  virtual void TypeSafeInitializeBuffers(
      const BuffersBase<Window16>& in,
      BuffersBase<WindowF>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Window16>& in,
                          BuffersBase<WindowF> *out) const noexcept;
};

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_INT16_TO_FLOAT_H_
