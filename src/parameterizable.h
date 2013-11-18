/*! @file parameterizable.h
 *  @brief Parameterizable abstract class.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_PARAMETERIZABLE_H_
#define SRC_PARAMETERIZABLE_H_

#include <memory>
#include <unordered_map>  // NOLINT(build/include_order)
#include "src/exceptions.h"

namespace sound_feature_extraction {

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

typedef std::unordered_map<std::string, ParameterTraits> SupportedParametersMap;
typedef std::unordered_map<std::string, std::string> ParametersMap;

class Parameterizable {
 public:
  // TODO(v.markovtsev): drop this hack when gcc 4.7.x is not supported
#if (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
  virtual ~Parameterizable() {}
#else
  virtual ~Parameterizable() = default;
#endif

  virtual const SupportedParametersMap& SupportedParameters() const noexcept
      = 0;

  virtual const ParametersMap& GetParameters() const noexcept = 0;

  virtual void SetParameters(const ParametersMap& parameters) = 0;

  static ParametersMap Parse(const std::string& line);
};

}  // namespace sound_feature_extraction

#endif  // SRC_PARAMETERIZABLE_H_
