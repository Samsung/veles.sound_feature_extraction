/*! @file rotate.cc
 *  @brief Rotate the buffers.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/rotate.h"

namespace sound_feature_extraction {
namespace transforms {

using RotateF = Rotate<float>;

REGISTER_TRANSFORM(RotateF);

}  // namespace transforms
}  // namespace sound_feature_extraction
