/*! @file simd_aware.cc
 *  @brief sound_feature_extraction::SimdAware::useSimd_ initialization.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */


#include "src/simd_aware.h"
#ifdef VALGRIND
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <valgrind.h>
#pragma GCC diagnostic pop
#else
#define RUNNING_ON_VALGRIND false
#endif

namespace sound_feature_extraction {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
static bool under_valgrind() noexcept {
  return RUNNING_ON_VALGRIND;
}
#pragma GCC diagnostic pop

bool SimdAware::use_simd_ = !under_valgrind();

}  // namespace sound_feature_extraction

