/*! @file attributes.h
 *  @brief This class adds an ability to store and retrieve attributes, that is,
 *  key-value pairs which are optional to the underlying object.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_ATTRIBUTES_H_
#define SRC_ATTRIBUTES_H_

#include <memory>
#include <unordered_map>
#include "src/exceptions.h"

namespace SoundFeatureExtraction {

class AttributeNotFoundException : public ExceptionBase {
 public:
  explicit AttributeNotFoundException(const std::string& name)
  : ExceptionBase(std::string("Attribute ") + name + " was not found.") {}
};

/// @brief Adds an ability to store and retrieve attributes, that is,
/// key-value pairs which are optional to the underlying object.
class Attributes {
 public:
  Attributes() : attrs_(std::make_shared<AttributesMap>()) {
  }

  virtual ~Attributes() {
  }

  template<typename T>
  void SetAttribute(const std::string& name,
                    const std::shared_ptr<T>& value) noexcept {
    (*attrs_)[name] = value;
  }

  /// @brief This overload accepts only plain old data types.
  template<typename T,
           class = typename std::enable_if<std::is_pod<T>::value>::type>
  void SetAttributeValue(const std::string& name,
                         const T& value) noexcept {
    auto ptr = std::make_shared<T>();
    *ptr = value;
    (*attrs_)[name] = ptr;
  }

  template<typename T>
  std::shared_ptr<T> GetAttribute(const std::string& name) const {
    auto iter = attrs_->find(name);
    if (iter == attrs_->end()) {
      throw AttributeNotFoundException(name);
    }
    return iter->second;
  }

  /// @brief This overload accepts only plain old data types.
  template<typename T,
           class = typename std::enable_if<std::is_pod<T>::value>::type>
  T GetAttributeValue(const std::string& name) const {
    auto iter = attrs_->find(name);
    if (iter == attrs_->end()) {
      throw AttributeNotFoundException(name);
    }
    return *std::static_pointer_cast<T>(iter->second);
  }

  bool HasAttribute(const std::string& name) const noexcept {
    return attrs_->find(name) != attrs_->end();
  }

 private:
  typedef std::unordered_map<std::string, std::shared_ptr<void>> AttributesMap;

  std::shared_ptr<AttributesMap> attrs_;
};

}  // namespace SoundFeatureExtraction
#endif  // SRC_ATTRIBUTES_H_
