/*! @file intF_to_int32.h
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

#ifndef FLOAT_TO_INT32_H_
#define FLOAT_TO_INT32_H_

#include "src/format_converter_base.h"
#include "src/formats/raw_format.h"
#include "src/formats/window_format.h"

namespace SpeechFeatureExtraction {
namespace Formats {

class FloatToInt32Raw
    : public FormatConverterBase<RawFormatF, RawFormat32> {
 protected:
  virtual void TypeSafeInitializeBuffers(
      const BuffersBase<RawF>& in,
      BuffersBase<Raw32>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<RawF>& in,
                          BuffersBase<Raw32> *out) const noexcept;
};

class FloatToInt32Window
    : public FormatConverterBase<WindowFormatF, WindowFormat32> {
 protected:
  virtual void TypeSafeInitializeBuffers(
      const BuffersBase<WindowF>& in,
      BuffersBase<Window32>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<WindowF>& in,
                          BuffersBase<Window32> *out) const noexcept;
};

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_FLOAT_TO_INT32_H_
