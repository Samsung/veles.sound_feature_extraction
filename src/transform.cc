/*! @file transform.cc
 *  @brief Transform parent class definition.
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

#include "src/transform.h"
#include <cassert>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/algorithm/string/replace.hpp>
#pragma GCC diagnostic pop
#include "src/transform_registry.h"

using boost::algorithm::replace_all_copy;

namespace sound_feature_extraction {

bool Transform::BufferInvariant() const noexcept {
  return false;
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

}  // namespace sound_feature_extraction


