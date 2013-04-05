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
#include <string>
#include <unordered_map>

namespace SpeechFeatureExtraction {

/// @brief Adds an ability to store and retrieve attributes, that is,
/// key-value pairs which are optional to the underlying object.
class Attributes {
 public:
  virtual ~Attributes() noexcept {
  }

  template<typename T>
  void SetAttribute(const std::string& name,
                    const std::shared_ptr<T>& value) noexcept {
    (*attrs_)[name] = value;
  }

  template<typename T>
  std::shared_ptr<T> GetAttribute(const std::string& name) noexcept {
    return (*attrs_)[name];
  }

 private:
  std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<void>>>
  attrs_;
};

}  // namespace SpeechFeatureExtraction
#endif  // SRC_ATTRIBUTES_H_
