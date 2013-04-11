/*! @file format_converter.cc
 *  @brief Interface of a converter between formats.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/format_converter.h"
#include <assert.h>
#include <string>

namespace SoundFeatureExtraction {

std::string FormatConverter::Name(const BufferFormat& in,
                                  const BufferFormat& out) {
  return in.Id() + " -> " + out.Id();
}

const std::string& FormatConverter::Name() const noexcept {
  if (name_ == "") {
    name_ = Name(*InputFormat(), *OutputFormat());
  }
  return name_;
}

const std::string& FormatConverter::Description() const noexcept {
  if (description_ == "") {
    description_ = std::string("Converts data format from ") +
        InputFormat()->Id() + " to " + OutputFormat()->Id() + ".";
  }
  return description_;
}

const std::unordered_map<std::string, ParameterTraits>&
FormatConverter::SupportedParameters() const noexcept {
  static const std::unordered_map<std::string, ParameterTraits> sp;
  return sp;
}

const std::unordered_map<std::string, std::string>&
FormatConverter::GetParameters() const noexcept {
  static const std::unordered_map<std::string, std::string> cp;
  return cp;
}

void FormatConverter::SetParameters(
  const std::unordered_map<std::string, std::string>&) {
}

void FormatConverter::Initialize() const noexcept {
}

}  // namespace SoundFeatureExtraction
