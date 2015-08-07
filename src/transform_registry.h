/*! @file transform_registry.h
 *  @brief Transform factory's and registry function's declarations.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright © 2013 Samsung R&D Institute Russia
 *
 *  @section License
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 */

#ifndef SRC_TRANSFORM_REGISTRY_H_
#define SRC_TRANSFORM_REGISTRY_H_

#include "src/transform.h"

namespace sound_feature_extraction {

/// @brief Meyers singleton ideal for C++11.
class TransformFactory {
  template <class T>
  friend class RegisterTransform;
 public:
  typedef std::function<std::shared_ptr<Transform>(void)> TransformConstructor;

  typedef std::unordered_map<std::string,
                             std::unordered_map<std::string,
                                                TransformConstructor>>
      FactoryMap;

  /// @brief Returns a unique instance of TransformFactory class.
  static const TransformFactory& Instance();

  /// @brief Returns the hash map which stores the registered transforms.
  const FactoryMap& Map() const;

  /// @brief Prints the names of registered transforms to stdout.
  void PrintRegisteredTransforms() const;

 private:
  TransformFactory();
  ~TransformFactory();
  TransformFactory(const TransformFactory&) = delete;
  TransformFactory& operator=(const TransformFactory&) = delete;

  static TransformFactory& InstanceRW();

  FactoryMap& MapRW();

  FactoryMap map_;
};

/// @brief Helper class used to register transforms. Usually, you do not
/// want to use it explicitly but rather through REGISTER_TRANSFORM macro.
template<class T>
class RegisterTransform {
 public:
  /// @brief This function is called during the execution of static
  /// constructors of global RegisterTransform class instances in each of the
  /// transform's object file (declared in .cc using REGISTER_TRANSFORM macro).
  /// @details Using TransformFactory lazy singleton is safe even during library
  /// load process, thanks to guaranteed local static variables behavior.
  /// @note Never use any global or static variables in your transform's
  /// default constructor! The order of execution of static constructors is
  /// undefined.
  RegisterTransform() {
    TransformFactory::FactoryMap& map = TransformFactory::InstanceRW().MapRW();
    // Instantiate a new instance of the transform "T"
    T t;
    // Insert the constructor function make_shared<T> -> shared_ptr<T>
    map[t.Name()].insert({t.InputFormat()->Id(), std::make_shared<T>});
  }
};

/// @brief Transform registration utility.
/// @param T The type name of your transform class.
#define REGISTER_TRANSFORM(T) RegisterTransform<T> T##RegistryInstance

}  // namespace sound_feature_extraction
#endif  // SRC_TRANSFORM_REGISTRY_H_
