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
#include <cassert>
#include <string>

namespace sound_feature_extraction {

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

const SupportedParametersMap&
FormatConverter::SupportedParameters() const noexcept {
  static const SupportedParametersMap sp;
  return sp;
}

const ParametersMap&
FormatConverter::GetParameters() const noexcept {
  static const ParametersMap cp;
  return cp;
}

void FormatConverter::SetParameters(
  const ParametersMap&) {
}

void FormatConverter::Initialize() const {
}

}  // namespace sound_feature_extraction
