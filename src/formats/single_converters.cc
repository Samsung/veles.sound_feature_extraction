/*! @file single_converters.cc
 *  @brief Converters for SingleFormat.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/formats/single_converters.h"

namespace SoundFeatureExtraction {
namespace Formats {

void Int32ToFloatConverter::Do(const int32_t& in,
                               float *out) const noexcept {
  *out = static_cast<float>(in);
};

void FloatToInt32Converter::Do(const float& in,
                               int32_t *out) const noexcept {
  *out = static_cast<int32_t>(in);
};

REGISTER_TRANSFORM(Int32ToFloatConverter);
REGISTER_TRANSFORM(FloatToInt32Converter);

}  // namespace Formats
}  // namespace SoundFeatureExtraction


