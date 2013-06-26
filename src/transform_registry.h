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

#include "src/transform.h"

namespace SoundFeatureExtraction {

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

class InverseParameterAware {
 public:
  static const std::string kInverseParameterName;

  /// @brief Checks for the support of doing an inverse transform.
  /// @details This function tests the transform's parameters for
  /// the existence of INVERSE_PARAMETER.
  static bool HasInverseTransform(const Transform& transform) noexcept;
};

/// @brief Helper class used to register transforms. Usually, you do not
/// want to use it explicitly but rather through REGISTER_TRANSFORM macro.
template<class T>
class RegisterTransform : InverseParameterAware {
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
    // Next line instantiates a new instance of the transform "T"
    T t;
    // Insert the constructor functor
    map[t.Name()].insert(std::make_pair(
        t.InputFormat()->Id(), []() {
      auto ptr = std::make_shared<T>();
      return ptr;
    }));
    // Insert the inverse constructor functor, if needed
    if (HasInverseTransform(t)) {
      map[std::string("I") + t.Name()].insert(std::make_pair(
          t.OutputFormat()->Id(), []() {
        auto ptr = std::make_shared<T>();
        ptr->SetParameter(kInverseParameterName, "true");
        return ptr;
      }));
    }
  }
};

/// @brief Transform registration utility.
/// @param T The type name of your transform class.
#define REGISTER_TRANSFORM(T) RegisterTransform<T> T##RegistryInstance

}  // namespace SoundFeatureExtraction
#endif  // SRC_TRANSFORM_REGISTRY_H_
