/*! @file identity.cc
 *  @brief Just copies the input.
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

#include "src/transforms/identity.h"
#include "src/transform_registry.h"

namespace sound_feature_extraction {
namespace transforms {

constexpr const char* Identity::kName;

Identity::Identity()
    : input_format_(std::make_shared<IdentityFormat>()),
      output_format_(std::make_shared<IdentityFormat>()) {
}

const std::string& Identity::Name() const noexcept {
  static const std::string name(kName);
  return name;
}

const std::string& Identity::Description() const noexcept {
  static const std::string desc("Copy the input to output.");
  return desc;
}

const std::shared_ptr<BufferFormat> Identity::InputFormat() const noexcept {
  return input_format_;
}

size_t Identity::SetInputFormat(const std::shared_ptr<BufferFormat>& format,
                              size_t buffersCount) {
  input_format_ = format;
  output_format_ = format;
  return buffersCount;
}

const std::shared_ptr<BufferFormat> Identity::OutputFormat() const noexcept {
  return output_format_;
}

void Identity::Initialize() const {
}

void Identity::Do(const Buffers& in, Buffers* out) const noexcept {
  *out = in;
}

std::shared_ptr<Buffers> Identity::CreateOutputBuffers(
    size_t count, void* reusedMemory) const noexcept {
  return std::make_shared<Buffers>(output_format_, count, reusedMemory);
}

const SupportedParametersMap&
    Identity::SupportedParameters() const noexcept {
  static const SupportedParametersMap sp;
  return sp;
}

const ParametersMap&
Identity::GetParameters() const noexcept {
  static const ParametersMap p;
  return p;
}

void Identity::SetParameters(
    const ParametersMap&) {
}

REGISTER_TRANSFORM(Identity);

}  // namespace transforms
}  // namespace sound_feature_extraction
