/*! @file features_parser.h
 *  @brief Features string functions.
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

namespace features {

class ParseFeaturesException : public ExceptionBase {
 public:
  explicit ParseFeaturesException(size_t index)
  : ExceptionBase("Syntax error on feature at index " + std::to_string(index) +
                  ".") {}
};

/// @brief Splits a list of feature text descriptions to
/// the name-parameters table. The syntax is as follows:
/// <feature name>[<transform name>(<parameter name>=<value>,
/// <parameter name>=<value>), <transform name>]
RawFeaturesMap Parse(const std::vector<std::string>& rawFeatures);

}  // namespace features

}  // namespace sound_feature_extraction


#endif  // SRC_FEATURES_PARSER_H_
