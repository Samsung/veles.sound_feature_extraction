/*! @file common.cc
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

#include "src/transforms/common.h"

namespace SoundFeatureExtraction {

template class TransformBase<Formats::RawFormatF, Formats::RawFormatF, true>;
template class TransformBase<Formats::RawFormatF, Formats::RawFormatF, false>;
template class UniformFormatTransform<Formats::RawFormatF, false>;
template class OmpTransformBase<Formats::RawFormatF, Formats::RawFormatF, true>;
template class OmpTransformBase<Formats::RawFormatF, Formats::RawFormatF, false>;
template class OmpUniformFormatTransform<Formats::RawFormatF, false>;

}  // namespace SoundFeatureExtraction
