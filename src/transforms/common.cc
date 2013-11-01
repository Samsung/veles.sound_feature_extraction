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

namespace sound_feature_extraction {

template class TransformBase<formats::ArrayFormatF, formats::ArrayFormatF>;
template class UniformFormatTransform<formats::ArrayFormatF>;
template class OmpTransformBaseBufferTypeProxy<formats::ArrayFormatF,
    formats::ArrayFormatF, formats::ArrayFormatF::BufferType,
    formats::ArrayFormatF::BufferType>;
template class OmpUniformFormatTransform<formats::ArrayFormatF>;

}  // namespace sound_feature_extraction
