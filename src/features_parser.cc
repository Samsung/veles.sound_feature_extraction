/*! @file features_parser.cc
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

#include "src/features_parser.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <boost/regex.hpp>
#pragma GCC diagnostic pop

namespace sound_feature_extraction {

namespace Features {

void AddToTransformsList(const std::string& str, size_t featureIndex,
                         RawTransformsList* transforms) {
  static const boost::regex name_regex("(^\\w+)");
  static const boost::regex parameters_regex("\\(([^\\)]*)\\)");
  static const boost::sregex_token_iterator empty;

  boost::sregex_token_iterator name_iterator(
      str.begin(), str.end(), name_regex, 1);
  if (name_iterator == empty) {
    throw ParseFeaturesException(featureIndex);
  }
  transforms->push_back(std::make_pair(
      *name_iterator++, ""));
  if (name_iterator != empty) {
    throw ParseFeaturesException(featureIndex);
  }
  boost::sregex_token_iterator parameters_iterator(
      str.begin(), str.end(), parameters_regex, 1);
  if (parameters_iterator != empty) {
    std::string pvalue = *parameters_iterator++;
    if (pvalue.size() > 0) {
      (*transforms)[transforms->size() - 1].second = pvalue;
    }
    if (parameters_iterator != empty) {
      throw ParseFeaturesException(featureIndex);
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
      throw ParseFeaturesException(index);
    }
    std::string fname = *feature_name_iterator++;
    if (feature_name_iterator != empty) {
      throw ParseFeaturesException(index);
    }
    ret.insert(std::make_pair(fname, RawTransformsList()));

    boost::sregex_token_iterator feature_transforms_iterator(
        str.begin(), str.end(), featureRegex, 2);
    if (feature_transforms_iterator == empty) {
      throw ParseFeaturesException(index);
    }
    auto transformsStr = std::string(",") + *feature_transforms_iterator++;
    if (feature_transforms_iterator != empty) {
      throw ParseFeaturesException(index);
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
      throw ParseFeaturesException(index);
    }
    AddToTransformsList(*feature_transforms_end_iterator++,
                        index,
                        &ret[fname]);
    if (feature_transforms_end_iterator != empty) {
      throw ParseFeaturesException(index);
    }
  }
  return ret;
}

}  // namespace Features

}  // namespace sound_feature_extraction
