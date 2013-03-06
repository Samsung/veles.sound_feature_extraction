/*! @file transform_registry.cc
 *  @brief Transform factory definition.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transform_registry.h"
#include <stdio.h>
#include <string>

namespace SpeechFeatureExtraction {

std::unordered_map<std::string,
                   std::unordered_map<std::string,
                                      TransformConstructor>>
TransformFactory;

void PrintRegisteredTransforms() {
  for (auto tit : SpeechFeatureExtraction::TransformFactory) {
    printf("%s\n", tit.first.c_str());
  }
}

}  // namespace SpeechFeatureExtraction
