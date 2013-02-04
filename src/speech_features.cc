/*! @file speech_features.cc
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

#include "src/speech_features.h"
#include <boost/regex.hpp>

namespace SpeechFeatureExtraction {

namespace Features {

void AddToTransformsList(const std::string& str, size_t index,
                         RawTransformsList* transforms) {
  static const boost::sregex_token_iterator empty;
  boost::regex transformRegex("(\\w+\\d*)\\s*(\\([^\\)]\\)|)\\s*,?\\s*");
  boost::sregex_token_iterator transformNameIterator(
      str.begin(), str.end(), transformRegex, 1);
  if (transformNameIterator == empty) {
    throw new ParseFeaturesException(index);
  }
  while (transformNameIterator != empty) {
    transforms->push_back(std::make_pair(
        *transformNameIterator++, ""));
  }
  boost::sregex_token_iterator transformParametersIterator(
      str.begin(), str.end(), transformRegex, 2);
  int i = 0;
  while (transformParametersIterator != empty) {
      (*transforms)[i++].second = *transformParametersIterator++;
  }
}

RawFeaturesMap Parse(const std::vector<std::string>& rawFeatures)
throw (ParseFeaturesException) {
  RawFeaturesMap ret;

  boost::regex featuresRegex("(\\w+\\d*)\\["
                             "(\\w+\\d*\\s*(\\([^\\)]\\))?\\s*,\\s*)+"
                               "(\\w+\\d*\\s*(\\([^\\)]\\))?)\\s*"
                             "\\])\\s*");
  static const boost::sregex_token_iterator empty;

  for (size_t index = 0; index < rawFeatures.size(); index++) {
    auto str = rawFeatures[index];
    boost::sregex_token_iterator featureNameIterator(
        str.begin(), str.end(), featuresRegex, 1);
    if (featureNameIterator == empty) {
      throw new ParseFeaturesException(index);
    }
    auto fname = *featureNameIterator++;
    if (featureNameIterator != empty) {
      throw new ParseFeaturesException(index);
    }
    ret.insert(std::make_pair(fname, RawTransformsList()));
    boost::sregex_token_iterator featureTransformsIterator(
        str.begin(), str.end(), featuresRegex, 2);
    while (featureTransformsIterator != empty) {
      auto transformStr = *featureTransformsIterator++;
      AddToTransformsList(transformStr, index, &ret[fname]);
    }
    boost::sregex_token_iterator featureTransformsIteratorEnd(
        str.begin(), str.end(), featuresRegex, 3);
    if (featureTransformsIteratorEnd == empty) {
      throw new ParseFeaturesException(index);
    }
    AddToTransformsList(*featureTransformsIteratorEnd++,
                        index,
                        &ret[fname]);
    if (featureTransformsIteratorEnd != empty) {
      throw new ParseFeaturesException(index);
    }
  }
  return std::move(ret);
}

}  // namespace Features

}  // namespace SpeechFeatureExtraction
