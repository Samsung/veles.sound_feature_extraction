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

#ifndef SRC_TRANSFORM_REGISTRY_H_
#define SRC_TRANSFORM_REGISTRY_H_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>  // NOLINT(build/include_order)
#include "src/transform.h"

namespace SpeechFeatureExtraction {

typedef std::function<std::shared_ptr<Transform>(void)> TransformConstructor;

extern std::unordered_map<std::string,
                          std::unordered_map<std::string,
                                             TransformConstructor>>
TransformFactory;

template<class T>
class RegisterTransform {
 public:
  RegisterTransform() {
    T t;
    if (TransformFactory.find(t.Name()) == TransformFactory.end()) {
      TransformFactory.insert(std::make_pair(
          t.Name(),
          std::unordered_map<std::string, TransformConstructor>()));
      if (t.HasInverse()) {
        TransformFactory.insert(std::make_pair(
            std::string("I") + t.Name(),
            std::unordered_map<std::string, TransformConstructor>()));
      }
    }
    TransformFactory[t.Name()].insert(std::make_pair(
        t.InputFormat()->Id(), []() {
      auto ptr = std::make_shared<T>();
      ptr->SetParameters({ {"inverse", "false"} });
      return ptr;
    }));
    if (t.HasInverse()) {
      TransformFactory[std::string("I") + t.Name()].insert(std::make_pair(
          t.OutputFormat()->Id(), []() {
        auto ptr = std::make_shared<T>();
        ptr->SetParameters({ {"inverse", "true"} });
        return ptr;
      }));
    }
  }
};

#define REGISTER_TRANSFORM(T) RegisterTransform<T> T##RegistryInstance

void PrintRegisteredTransforms();

}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORM_REGISTRY_H_
