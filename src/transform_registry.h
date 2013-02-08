/*! @file transform_registry.h
 *  @brief Transform factory's and registry function's declarations.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef TRANSFORM_REGISTER_H_
#define TRANSFORM_REGISTER_H_

#include <functional>
#include <memory>
#include <unordered_map>
#include "src/transform.h"

namespace SpeechFeatureExtraction {

extern std::unordered_map<std::string,
                          std::function<std::shared_ptr<Transform>(void)>>
    TransformFactory;

template<class T>
class RegisterTransform {
 public:
  RegisterTransform() {
    T t;
    TransformFactory.insert(
        std::make_pair(t.Name(), []() { return std::make_shared<T>(); }));
    if (t.HasInverse()) {
      TransformFactory.insert(
        std::make_pair(std::string("I") + t.Name(),
                       []() {
        auto ptr = std::make_shared<T>();
        ptr->SetParameters({ {"inverse", "true"} });
        return ptr;
      }));
    }
  }
};

#define REGISTER_TRANSFORM(T) RegisterTransform<T> T##RegistryInstance

}  // namespace SpeechFeatureExtraction
#endif  // INCLUDE_TRANSFORM_REGISTER_H_
