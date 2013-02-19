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

namespace SpeechFeatureExtraction {

std::string FormatConverter::Name(const BufferFormat& in,
                                  const BufferFormat& out) {
  return in.Id() + " -> " + out.Id();
}

const std::string& FormatConverter::Name() const noexcept {
  static const auto name = Name(InputFormat(), OutputFormat());
  return name;
}

void FormatConverter::SetInputFormat(const BufferFormat&) {
  std::unexpected();
}

const std::unordered_map<std::string, ParameterTraits>&
FormatConverter::SupportedParameters() const noexcept {
  static const std::unordered_map<std::string, ParameterTraits> sp;
  return sp;
}

const std::unordered_map<std::string, std::string>&
FormatConverter::CurrentParameters() const noexcept {
  static const std::unordered_map<std::string, std::string> cp;
  return cp;
}

void FormatConverter::SetParameters(
   const std::unordered_map<std::string, std::string>&) {
}

void FormatConverter::Initialize() const noexcept {
}

}  // namespace SpeechFeatureExtraction
