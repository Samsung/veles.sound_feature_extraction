/*! @file identity.cc
 *  @brief Just copies the input.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/identity.h"
#include "src/transform_registry.h"

namespace sound_feature_extraction {
namespace transforms {

Identity::Identity()
    : input_format_(std::make_shared<IdentityFormat>()),
      output_format_(std::make_shared<IdentityFormat>()) {
}

const std::string& Identity::Name() const noexcept {
  static const std::string name("Identity");
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

const std::unordered_map<std::string, ParameterTraits>&
    Identity::SupportedParameters() const noexcept {
  static const std::unordered_map<std::string, ParameterTraits> sp;
  return sp;
}

const std::unordered_map<std::string, std::string>&
Identity::GetParameters() const noexcept {
  static const std::unordered_map<std::string, std::string> p;
  return p;
}

void Identity::SetParameters(
    const std::unordered_map<std::string, std::string>&) {
}

REGISTER_TRANSFORM(Identity);

}  // namespace transforms
}  // namespace sound_feature_extraction
