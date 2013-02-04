/*! @file transform.cc
 *  @brief Transform parent class definition.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transform.h"
#include <assert.h>
#include "src/transform_registry.h"

namespace SpeechFeatureExtraction {

const std::unordered_map<std::string,
                         std::function<bool(const std::string&)>>&
Transform::DependencyParametersCheck() const noexcept {
  static const std::unordered_map<std::string,
                                  std::function<bool(const std::string&)>> m;
  return m;
}

const std::unordered_map<std::string, std::string>&
Transform::DependencyParameters() const noexcept {
  static const std::unordered_map<std::string, std::string> m;
  return m;
}

std::shared_ptr<Transform> Transform::Clone() const {
  auto copy = TransformFactory[this->Name()]();
  copy->SetParameters(this->CurrentParameters());
  return copy;
}

bool Transform::operator==(const Transform& other) const {
  if (this->Name() != other.Name()) return false;
  assert(CurrentParameters().size() == other.CurrentParameters().size());
  for (auto p : CurrentParameters()) {
    if (other.CurrentParameters().find(p.first)->second != p.second) {
      return false;
    }
  }
  return true;
}

}  // namespace SpeechFeatureExtraction


