/*! @file api.cc
 *  @brief Speech Feature Extraction interface implementation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <spfextr/api.h>
#include <stddef.h>
#include "src/speech_features.h"
#include "src/transform_tree.h"

using SpeechFeatureExtraction::ChainNameAlreadyExistsException;
using SpeechFeatureExtraction::TransformNotRegisteredException;
using SpeechFeatureExtraction::ChainAlreadyExistsException;
using SpeechFeatureExtraction::IncompatibleTransformFormatException;
using SpeechFeatureExtraction::RawFeaturesMap;
using SpeechFeatureExtraction::Features::ParseFeaturesException;
using SpeechFeatureExtraction::TransformTree;
using SpeechFeatureExtraction::Formats::RawFormat16;

extern "C" {

struct FeaturesConfiguration {
  std::shared_ptr<TransformTree> Tree;
};

FeaturesConfiguration *setup_features_extraction(
    const char *const *features, int featuresCount,
    size_t bufferSize, int samplingRate) {
  if (features == nullptr) {
    fprintf(stderr, "features is null");
    return nullptr;
  }
  if (featuresCount < 0) {
    fprintf(stderr, "featuresCount is negative (%i)\n", featuresCount);
    return nullptr;
  }
  if (featuresCount > MAX_FEATURES_COUNT) {
    fprintf(stderr, "featuresCount is too big (%i > MAX_FEATURES_COUNT=%i)\n",
            featuresCount, MAX_FEATURES_COUNT);
    return nullptr;
  }
  std::vector<std::string> lines;
  for (int i = 0; i < featuresCount; i++) {
    if (features[i] == nullptr) {
      fprintf(stderr, "features[%i] is null", i);
      return nullptr;
    }
    lines.push_back(features[i]);
  }
  RawFeaturesMap featmap;
  try {
    featmap = SpeechFeatureExtraction::Features::Parse(lines);
  }
  catch(ParseFeaturesException* pfe) {
    fprintf(stderr, "Failed to parse features. %s\n", pfe->what());
    delete pfe;
    return nullptr;
  }

  RawFormat16 format(bufferSize, samplingRate);
  auto fconfig = new FeaturesConfiguration();
  fconfig->Tree = std::make_shared<TransformTree>(format);
  for (auto featpair : featmap) {
    try {
      fconfig->Tree->AddChain(featpair.first, featpair.second);
    }
    catch (ChainNameAlreadyExistsException* cnaee) {
      fprintf(stderr, "Failed to construct the transform tree. %s\n",
              cnaee->what());
      delete cnaee;
      return nullptr;
    }
    catch (TransformNotRegisteredException* tnre) {
      fprintf(stderr, "Failed to construct the transform tree. %s\n",
              tnre->what());
      delete tnre;
      return nullptr;
    }
    catch (ChainAlreadyExistsException* caee) {
      fprintf(stderr, "Failed to construct the transform tree. %s\n",
              caee->what());
      delete caee;
      return nullptr;
    }
    catch (IncompatibleTransformFormatException* itfe) {
      fprintf(stderr, "Failed to construct the transform tree. %s\n",
              itfe->what());
      delete itfe;
      return nullptr;
    }
  }
  fconfig->Tree->PrepareForExecution();
  return fconfig;
}

FeatureExtractionResult extract_speech_features(
    const FeaturesConfiguration *fc, const int16_t *buffer,
    void ***results) {
  return FEATURE_EXTRACTION_RESULT_OK;
}

void destroy_features_configuration(FeaturesConfiguration* fc) {
  delete fc;
}

void free_results(void **results, int featuresCount) {
  for (int i = 0; i < featuresCount; i++) {
    free(results[i]);
  }
  free(results);
}

}
