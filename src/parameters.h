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

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include <memory>
#include <string>
#include <unordered_map>
#include "src/exceptions.h"

namespace SpeechFeatureExtraction {

struct ParameterTraits {
  std::string Description;
  std::string DefaultValue;
};

namespace Parameters {

class ParseParametersException : public ExceptionBase {
 public:
  ParseParametersException(const std::string& text, int column)
  : ExceptionBase("Syntax error on symbol at index " + std::to_string(column) +
                    ": \"..." + text.substr(column, 20) +
                    (static_cast<size_t>(column) + 10 >= text.size()?
                        "\"." : "...\" .")
                  ) {}
};

std::unordered_map<std::string, std::string> Parse(const std::string& line);

}  // namespace Parameters

}  // namespace SpeechFeatureExtraction

#endif  // INCLUDE_PARAMETERS_H_
