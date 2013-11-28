/*! @file singles_to_array.cc
 *  @brief Merge all single-s to one solid array.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/singles_to_array.h"

namespace sound_feature_extraction {
namespace transforms {

using SinglesToArrayF = SinglesToArray<float>;

REGISTER_TRANSFORM(SinglesToArrayF);

}  // namespace transforms
}  // namespace sound_feature_extraction
