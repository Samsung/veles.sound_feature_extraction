/*! @file features_parser.h
 *  @brief Features string functions.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_FEATURES_PARSER_H_
#define SRC_FEATURES_PARSER_H_

#include <unordered_map>  // NOLINT(build/include_order)
#include <vector>
#include "src/exceptions.h"

namespace sound_feature_extraction {

typedef std::vector<
    std::pair<
      // Transform name
      std::string,
      // Transform parameters (raw text)
      std::string
    >
  > RawTransformsList;

typedef std::unordered_map<
    // Feature name
    std::string,
    // Transforms list
    RawTransformsList
  > RawFeaturesMap;

namespace Features {

class ParseFeaturesException : public ExceptionBase {
 public:
  explicit ParseFeaturesException(size_t index)
  : ExceptionBase("Syntax error on feature at index " + std::to_string(index) +
                  ".") {}
};

/// @brief Splits a list of feature text descriptions to
/// the name-parameters table. The syntax is as follows:
/// <feature name>[<transform name>(<parameter name>=<value>, <parameter name>=<value>), <transform name>]
RawFeaturesMap Parse(const std::vector<std::string>& rawFeatures);

}  // namespace Features

}  // namespace sound_feature_extraction


#endif  // SRC_FEATURES_PARSER_H_
