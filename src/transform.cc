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

namespace SoundFeatureExtraction {

const BuffersCountChange BuffersCountChange::Identity;

BuffersCountChange::BuffersCountChange() noexcept
    : calc_([=](size_t inSize) { return inSize; }) {
}

BuffersCountChange::BuffersCountChange(int constant) noexcept
    : calc_([=](size_t) { return constant; }) {
}

BuffersCountChange::BuffersCountChange(int num, int den) noexcept
    : calc_([=](size_t inSize) {
              return inSize * num / den;
            }) {
}

BuffersCountChange::BuffersCountChange(
    const std::function<size_t(size_t)>& calc) noexcept
    : calc_(calc) {
}

bool BuffersCountChange::operator==(
    const BuffersCountChange& other) const noexcept {
  return calc_ == nullptr && other.calc_ == nullptr;
}

size_t BuffersCountChange::operator()(size_t count) const noexcept {
  return calc_(count);
}

void Transform::UpdateInputFormat(const std::shared_ptr<BufferFormat>& format) {
  buffersCountChange_ = SetInputFormat(format);
}

BuffersCountChange Transform::CalculateBuffersCountChange() const noexcept {
  return buffersCountChange_;
}

std::shared_ptr<Transform> Transform::Clone() const noexcept {
  auto copy = TransformFactory::Instance().Map()
      .find(this->Name())->second
      .find(this->InputFormat()->Id())->second();
  copy->SetParameters(this->GetParameters());
  return copy;
}

bool Transform::operator==(const Transform& other) const noexcept {
  if (this->Name() != other.Name()) return false;
  assert(GetParameters().size() == other.GetParameters().size());
  for (auto p : GetParameters()) {
    if (other.GetParameters().find(p.first)->second != p.second) {
      return false;
    }
  }
  return true;
}

std::string Transform::SafeName() const noexcept {
  return replace_all_copy(replace_all_copy(replace_all_copy(replace_all_copy(
      replace_all_copy(replace_all_copy(
      Name(), " ", ""), "->", "To"), "!", ""), ">", "_"), "<", "_"),
      "*", "Array");
}

std::string Transform::HtmlEscapedName() const noexcept {
  return replace_all_copy(replace_all_copy(replace_all_copy(replace_all_copy(
      Name(), "&", "&amp;"), ">", "&gt;"), "<", "&lt;"), "!", "");
}

}  // namespace SoundFeatureExtraction


