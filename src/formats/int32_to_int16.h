/*! @file int32_to_int16.h
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

#ifndef INT32_TO_INT16_H_
#define INT32_TO_INT16_H_

#include "src/format_converter_base.h"
#include "src/formats/raw_format.h"
#include "src/formats/window_format.h"

namespace SpeechFeatureExtraction {
namespace Formats {

class Int32ToInt16Raw
    : public FormatConverterBase<RawFormat32, RawFormat16> {
 protected:
  virtual void TypeSafeInitializeBuffers(
      const BuffersBase<Raw32>& in,
      BuffersBase<Raw16>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Raw32>& in,
                          BuffersBase<Raw16> *out) const noexcept;
};

class Int32ToInt16Window
    : public FormatConverterBase<WindowFormat32, WindowFormat16> {
 protected:
  virtual void TypeSafeInitializeBuffers(
      const BuffersBase<Window32>& in,
      BuffersBase<Window16>* buffers) const noexcept;

  virtual void TypeSafeDo(const BuffersBase<Window32>& in,
                          BuffersBase<Window16> *out) const noexcept;
};

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_INT32_TO_INT16_H_
