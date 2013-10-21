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

namespace SoundFeatureExtraction {

extern template class TransformBase<Formats::ArrayFormatF,
                                    Formats::ArrayFormatF>;
extern template class UniformFormatTransform<Formats::ArrayFormatF>;
extern template class OmpTransformBaseBufferTypeProxy<Formats::ArrayFormatF,
    Formats::ArrayFormatF, Formats::ArrayFormatF::BufferType,
    Formats::ArrayFormatF::BufferType>;
extern template class OmpUniformFormatTransform<Formats::ArrayFormatF>;

}  // namespace SoundFeatureExtraction

#endif  // SRC_TRANSFORMS_COMMON_H_
