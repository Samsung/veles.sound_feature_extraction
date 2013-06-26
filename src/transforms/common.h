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

#include "src/formats/raw_format.h"
#include "src/omp_transform_base.h"

namespace SoundFeatureExtraction {

extern template class TransformBase<Formats::RawFormatF, Formats::RawFormatF, true>;
extern template class TransformBase<Formats::RawFormatF, Formats::RawFormatF, false>;
extern template class UniformFormatTransform<Formats::RawFormatF, false>;
extern template class OmpTransformBase<Formats::RawFormatF, Formats::RawFormatF, true>;
extern template class OmpTransformBase<Formats::RawFormatF, Formats::RawFormatF, false>;
extern template class OmpUniformFormatTransform<Formats::RawFormatF, false>;

}  // namespace SoundFeatureExtraction

#endif  // SRC_TRANSFORMS_COMMON_H_
