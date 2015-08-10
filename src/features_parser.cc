/*! @file features_parser.cc
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

#include "src/features_parser.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <boost/regex.hpp>
#pragma GCC diagnostic pop

namespace sound_feature_extraction {

namespace features {

void AddToTransformsList(const std::string& str, size_t featureIndex,
                         RawTransformsList* transforms) {
  static const boost::regex name_regex("(^\\w+)");
  static const boost::regex parameters_regex("\\(([^\\)]*)\\)");
  static const boost::sregex_token_iterator empty;

  boost::sregex_token_iterator name_iterator(
      str.begin(), str.end(), name_regex, 1);
  if (name_iterator == empty) {
    THROW_PFE(str, featureIndex);
  }
  transforms->push_back(std::make_pair(*name_iterator++, ""));
  if (name_iterator != empty) {
    THROW_PFE(transforms->back().first, featureIndex);
  }
  boost::sregex_token_iterator parameters_iterator(
      str.begin(), str.end(), parameters_regex, 1);
  if (parameters_iterator != empty) {
    std::string pvalue = *parameters_iterator++;
    if (pvalue.size() > 0) {
      transforms->back().second = pvalue;
    }
    if (parameters_iterator != empty) {
      THROW_PFE(transforms->back().first, featureIndex);
    }
  }
}

RawFeaturesMap Parse(const std::vector<std::string>& rawFeatures) {
  RawFeaturesMap ret;

  static const boost::regex featureRegex(
      "(^\\w+)\\s*\\[([^\\]]+)\\]\\s*");
  static const boost::regex transformsRegex(
      "(?<=,)\\s*(\\w+\\s*(\\([^\\)]*\\))?)(?=\\s*,\\s*)");
  static const boost::regex transformsEndRegex(
      "(\\w+\\s*(\\([^\\)]*\\))?)\\s*$");
  static const boost::sregex_token_iterator empty;

  for (size_t index = 0; index < rawFeatures.size(); index++) {
    auto str = rawFeatures[index];

    boost::sregex_token_iterator feature_name_iterator(
        str.begin(), str.end(), featureRegex, 1);
    if (feature_name_iterator == empty) {
      THROW_PFE(str, index);
    }
    std::string fname = *feature_name_iterator++;
    if (feature_name_iterator != empty) {
      THROW_PFE(str, index);
    }
    ret.insert(std::make_pair(fname, RawTransformsList()));

    boost::sregex_token_iterator feature_transforms_iterator(
        str.begin(), str.end(), featureRegex, 2);
    if (feature_transforms_iterator == empty) {
      THROW_PFE(str, index);
    }
    auto transformsStr = std::string(",") + *feature_transforms_iterator++;
    if (feature_transforms_iterator != empty) {
      THROW_PFE(str, index);
    }

    boost::sregex_token_iterator feature_each_transform_iterator(
        transformsStr.begin(), transformsStr.end(), transformsRegex, 1);
    while (feature_each_transform_iterator != empty) {
      AddToTransformsList(*feature_each_transform_iterator++, index,
                          &ret[fname]);
    }

    boost::sregex_token_iterator feature_transforms_end_iterator(
        transformsStr.begin(), transformsStr.end(), transformsEndRegex, 1);
    if (feature_transforms_end_iterator == empty) {
      THROW_PFE(str, index);
    }
    AddToTransformsList(*feature_transforms_end_iterator++,
                        index,
                        &ret[fname]);
    if (feature_transforms_end_iterator != empty) {
      THROW_PFE(str, index);
    }
  }
  return ret;
}

}  // namespace features

}  // namespace sound_feature_extraction
