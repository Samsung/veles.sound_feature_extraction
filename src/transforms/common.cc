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

template class TransformBase<Formats::ArrayFormatF, Formats::ArrayFormatF>;
template class UniformFormatTransform<Formats::ArrayFormatF>;
template class OmpTransformBaseBufferTypeProxy<Formats::ArrayFormatF,
    Formats::ArrayFormatF, Formats::ArrayFormatF::BufferType,
    Formats::ArrayFormatF::BufferType>;
template class OmpUniformFormatTransform<Formats::ArrayFormatF>;

}  // namespace SoundFeatureExtraction
