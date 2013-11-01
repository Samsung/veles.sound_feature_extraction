/*! @file common.h
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

#ifndef SRC_TRANSFORMS_COMMON_H_
#define SRC_TRANSFORMS_COMMON_H_

#include "src/formats/array_format.h"
#include "src/omp_transform_base.h"

namespace sound_feature_extraction {

extern template class TransformBase<formats::ArrayFormatF,
                                    formats::ArrayFormatF>;
extern template class UniformFormatTransform<formats::ArrayFormatF>;
extern template class OmpTransformBaseBufferTypeProxy<formats::ArrayFormatF,
    formats::ArrayFormatF, formats::ArrayFormatF::BufferType,
    formats::ArrayFormatF::BufferType>;
extern template class OmpUniformFormatTransform<formats::ArrayFormatF>;

}  // namespace sound_feature_extraction

#endif  // SRC_TRANSFORMS_COMMON_H_
