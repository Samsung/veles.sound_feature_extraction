/*! @file parameters.h
 *  @brief Transform parameters related stuff.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_PARAMETERS_H_
#define SRC_PARAMETERS_H_

#include <memory>
#include <unordered_map>  // NOLINT(build/include_order)
#include "src/exceptions.h"

namespace SpeechFeatureExtraction {

class ParseParametersException : public ExceptionBase {
 public:
  ParseParametersException(const std::string& text, int column)
  : ExceptionBase("Syntax error on symbol at index " + std::to_string(column) +
                    ": \"..." + text.substr(column, 20) +
                    (static_cast<size_t>(column) + 10 >= text.size()?
                        "\"." : "...\" .")
                  ) {
  }
};

struct ParameterTraits {
  std::string Description;
  std::string DefaultValue;
};

class IParameterizable {
 public:
  virtual ~IParameterizable() {}

  virtual const std::unordered_map<std::string, ParameterTraits>&
  SupportedParameters() const noexcept = 0;

  virtual const std::unordered_map<std::string, std::string>&
  GetParameters() const noexcept = 0;

  virtual void SetParameters(
      const std::unordered_map<std::string, std::string>& parameters) = 0;

  static std::unordered_map<std::string, std::string> Parse(
      const std::string& line);
};

}  // namespace SpeechFeatureExtraction

#endif  // SRC_PARAMETERS_H_
