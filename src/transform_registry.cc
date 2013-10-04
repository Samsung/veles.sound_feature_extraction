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

namespace SoundFeatureExtraction {

TransformFactory::TransformFactory() {
}

TransformFactory::~TransformFactory() {
}

const TransformFactory& TransformFactory::Instance() {
  return InstanceRW();
}

const TransformFactory::FactoryMap& TransformFactory::Map() const {
  return map_;
}

TransformFactory& TransformFactory::InstanceRW() {
  static TransformFactory instance;
  return instance;
}

TransformFactory::FactoryMap& TransformFactory::MapRW() {
  return map_;
}

void TransformFactory::PrintRegisteredTransforms() const {
  for (auto tit : map_) {
    printf("%s\n", tit.first.c_str());
  }
}

bool InverseParameterAware::HasInverseTransform(
    const Transform& transform) noexcept {
  auto ip = transform.GetParameters().find(kInverseParameterName);
  return ip != transform.GetParameters().end();
}

}  // namespace SoundFeatureExtraction
