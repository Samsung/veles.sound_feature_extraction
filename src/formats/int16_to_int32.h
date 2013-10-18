/*! @file int16_to_int32.h
 *  @brief int16 to int32 converter.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FORMATS_INT16_TO_INT32_H_
#define SRC_FORMATS_INT16_TO_INT32_H_

#include "src/formats/array_format.h"
#include "src/formats/array_format_converter_base.h"

namespace SoundFeatureExtraction {
namespace Formats {

class Int16ToInt32Raw
    : public ArrayFormatConverterBase<ArrayFormat16, ArrayFormat32> {
 protected:
  virtual void Do(const int16_t* in,
                  int32_t* out) const noexcept override;
};

}  // namespace Formats
}  // namespace SoundFeatureExtraction
#endif  // SRC_FORMATS_INT16_TO_INT32_H_
