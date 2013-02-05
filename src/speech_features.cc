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

void AddToTransformsList(const std::string& str, size_t featureIndex,
                         RawTransformsList* transforms) {
  static const boost::regex nameRegex("(^\\w+\\d*)");
  static const boost::regex parametersRegex("\\(([^\\)]*)\\)");
  static const boost::sregex_token_iterator empty;

  boost::sregex_token_iterator nameIterator(
      str.begin(), str.end(), nameRegex, 1);
  if (nameIterator == empty) {
    throw new ParseFeaturesException(featureIndex);
  }
  transforms->push_back(std::make_pair(
      *nameIterator++, ""));
  if (nameIterator != empty) {
    throw new ParseFeaturesException(featureIndex);
  }
  boost::sregex_token_iterator parametersIterator(
      str.begin(), str.end(), parametersRegex, 1);
  if (parametersIterator != empty) {
    std::string pvalue = *parametersIterator++;
    if (pvalue.size() > 0) {
      (*transforms)[transforms->size() - 1].second = pvalue;
    }
    if (parametersIterator != empty) {
      throw new ParseFeaturesException(featureIndex);
    }
  }
}

RawFeaturesMap Parse(const std::vector<std::string>& rawFeatures)
throw (ParseFeaturesException) {
  RawFeaturesMap ret;

  static const boost::regex featureRegex(
      "(^\\w+\\d*)\\[([^\\]]+)\\]\\s*");
  static const boost::regex transformsRegex(
      "(\\w+\\d*\\s*(\\([^\\)]*\\))?)\\s*,\\s*");
  static const boost::regex transformsEndRegex(
      "(\\w+\\d*\\s*(\\([^\\)]*\\))?)\\s*$");
  static const boost::sregex_token_iterator empty;

  for (size_t index = 0; index < rawFeatures.size(); index++) {
    auto str = rawFeatures[index];

    boost::sregex_token_iterator featureNameIterator(
        str.begin(), str.end(), featureRegex, 1);
    if (featureNameIterator == empty) {
      throw new ParseFeaturesException(index);
    }
    std::string fname = *featureNameIterator++;
    if (featureNameIterator != empty) {
      throw new ParseFeaturesException(index);
    }
    ret.insert(std::make_pair(fname, RawTransformsList()));

    boost::sregex_token_iterator featureTransformsIterator(
        str.begin(), str.end(), featureRegex, 2);
    if (featureTransformsIterator == empty) {
      throw new ParseFeaturesException(index);
    }
    std::string transformsStr = *featureTransformsIterator++;
    if (featureTransformsIterator != empty) {
      throw new ParseFeaturesException(index);
    }

    boost::sregex_token_iterator featureEachTransformIterator(
        transformsStr.begin(), transformsStr.end(), transformsRegex, 1);
    while (featureEachTransformIterator != empty) {
      AddToTransformsList(*featureEachTransformIterator++, index, &ret[fname]);
    }

    boost::sregex_token_iterator featureTransformsEndIterator(
        transformsStr.begin(), transformsStr.end(), transformsEndRegex, 1);
    if (featureTransformsEndIterator == empty) {
      throw new ParseFeaturesException(index);
    }
    AddToTransformsList(*featureTransformsEndIterator++,
                        index,
                        &ret[fname]);
    if (featureTransformsEndIterator != empty) {
      throw new ParseFeaturesException(index);
    }
  }
  return std::move(ret);
}

}  // namespace Features

}  // namespace SpeechFeatureExtraction
