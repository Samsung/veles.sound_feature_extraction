/*! @file beat.cc
 *  @brief Beat detection.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/beat.h"


namespace SoundFeatureExtraction {
namespace Transforms {

using Beat16 = Beat<Formats::RawFormat16>;
using BeatF = Beat<Formats::RawFormatF>;

REGISTER_TRANSFORM(Beat16);
REGISTER_TRANSFORM(BeatF);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
