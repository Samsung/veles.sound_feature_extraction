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
#include <boost/algorithm/string/replace.hpp>
#include "src/transform_registry.h"

using boost::algorithm::replace_all_copy;

namespace SpeechFeatureExtraction {

std::shared_ptr<Transform> Transform::Clone() const noexcept {
  auto copy = TransformFactory[this->Name()][this->InputFormat().Id()]();
  copy->SetParameters(this->CurrentParameters());
  return copy;
}

bool Transform::operator==(const Transform& other) const noexcept {
  if (this->Name() != other.Name()) return false;
  assert(CurrentParameters().size() == other.CurrentParameters().size());
  for (auto p : CurrentParameters()) {
    if (other.CurrentParameters().find(p.first)->second != p.second) {
      return false;
    }
  }
  return true;
}

bool Transform::IsInverse() const noexcept {
  return CurrentParameters().find("inverse")->second == "true";
}

std::string Transform::SafeName() const noexcept {
  return replace_all_copy(replace_all_copy(replace_all_copy(replace_all_copy(
      replace_all_copy(
      Name(), " ", ""), "->", "To"), "!", ""), ">", "_"), "<", "_");
}

std::string Transform::HtmlEscapedName() const noexcept {
  return replace_all_copy(replace_all_copy(replace_all_copy(replace_all_copy(
      Name(), "&", "&amp;"), ">", "&gt;"), "<", "&lt;"), "!", "");
}

}  // namespace SpeechFeatureExtraction


