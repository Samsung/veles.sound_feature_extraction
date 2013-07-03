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

#define SOUNDFEATUREEXTRACTION_API_IMPLEMENTATION
#include <SoundFeatureExtraction/api.h>
#undef NOTNULL
#include <assert.h>
#include <stddef.h>
#include <fftf/api.h>
#include "src/features_parser.h"
#include "src/make_unique.h"
#include "src/safe_omp.h"
#include "src/simd_aware.h"
#include "src/transform_tree.h"
#include "src/transform_registry.h"

using SoundFeatureExtraction::Transform;
using SoundFeatureExtraction::TransformFactory;
using SoundFeatureExtraction::ChainNameAlreadyExistsException;
using SoundFeatureExtraction::TransformNotRegisteredException;
using SoundFeatureExtraction::ChainAlreadyExistsException;
using SoundFeatureExtraction::IncompatibleTransformFormatException;
using SoundFeatureExtraction::RawFeaturesMap;
using SoundFeatureExtraction::Features::ParseFeaturesException;
using SoundFeatureExtraction::TransformTree;
using SoundFeatureExtraction::Formats::RawFormat16;
using SoundFeatureExtraction::BuffersBase;
using SoundFeatureExtraction::Buffers;
using SoundFeatureExtraction::SimdAware;

extern "C" {
struct FeaturesConfiguration {
  std::unique_ptr<TransformTree> Tree;
};

#define BLAME(x) fprintf(stderr, "Error: " #x " is null (function %s, " \
                                 "line %i)\n", \
                         __FUNCTION__, __LINE__)

#define CHECK_NULL(x) do { if (x == nullptr) { \
    BLAME(x); \
    return; \
} } while(0)

#define CHECK_NULL_RET(x, ret) do { if (x == nullptr) { \
    BLAME(x); \
    return ret; \
} } while(0)

void copy_string(const std::string& str, char **ptr) {
  size_t size = str.size() + 1;
  *ptr = new char[size];
  memcpy(*ptr, str.c_str(), size);
}

void query_transforms_list(char ***names, int *listSize) {
  CHECK_NULL(names);
  CHECK_NULL(listSize);

  int i = 0;
  for (auto tc : TransformFactory::Instance().Map()) {
    if (tc.first.find("->") != std::string::npos) {
      continue;
    }
    i += tc.second.size();
  }

  *listSize = i;
  *names = new char*[*listSize];
  i = 0;
  for (auto tc : TransformFactory::Instance().Map()) {
    if (tc.first.find("->") != std::string::npos) {
      continue;
    }
    if (tc.second.size() == 1) {
      copy_string(tc.first, *names + i);
      i++;
    } else {
      for (auto tfc : tc.second) {
        std::string fullName(tc.first);
        fullName += " (";
        fullName += tfc.first;
        fullName += ")";
        copy_string(fullName, *names + i);
        i++;
      }
    }
  }
}

void destroy_transforms_list(char **names, int listSize) {
  CHECK_NULL(names);

  for (int i = 0; i < listSize; i++) {
    delete[] names[i];
  }
  delete[] names;
}

void query_format_converters_list(char ***inputFormats, char*** outputFormats,
                                  int *listSize) {
  CHECK_NULL(inputFormats);
  CHECK_NULL(outputFormats);
  CHECK_NULL(listSize);

  int i = 0;
  for (auto tc : TransformFactory::Instance().Map()) {
    if (tc.first.find("->") != std::string::npos) {
      i++;
    }
  }

  *listSize = i;
  *inputFormats = new char*[*listSize];
  *outputFormats = new char*[*listSize];
  i = 0;
  for (auto tc : TransformFactory::Instance().Map()) {
    if (tc.first.find("->") != std::string::npos) {
      auto transform = tc.second.begin()->second();
      copy_string(transform->InputFormat()->Id(), *inputFormats + i);
      copy_string(transform->OutputFormat()->Id(), *outputFormats + i);
      i++;
    }
  }
}

void destroy_format_converters_list(char **inputFormats, char** outputFormats,
                                    int listSize) {
  CHECK_NULL(inputFormats);
  CHECK_NULL(outputFormats);

  for (int i = 0; i < listSize; i++) {
    delete[] inputFormats[i];
    delete[] outputFormats[i];
  }
  delete[] inputFormats;
  delete[] outputFormats;
}

void query_transform_details(const char *name, char **description,
                             char **inputFormat, char **outputFormat,
                             char ***parameterNames,
                             char ***parameterDescriptions,
                             char ***parameterDefaultValues,
                             int *parametersCount) {
  CHECK_NULL(name);
  CHECK_NULL(description);
  CHECK_NULL(parameterNames);
  CHECK_NULL(parameterDescriptions);
  CHECK_NULL(parameterDefaultValues);
  CHECK_NULL(parametersCount);

  std::string fullName(name);
  std::string transformName(fullName);
  std::string formatName;
  auto bracePos = fullName.find('(');
  if (bracePos != fullName.npos) {
    transformName = fullName.substr(0, bracePos - 1);
    formatName = fullName.substr(bracePos + 1);
    formatName.resize(formatName.size() - 1);
  }
  auto tit = TransformFactory::Instance().Map().find(transformName);
  if (tit == TransformFactory::Instance().Map().end()) {
    fprintf(stderr, "Error: transform %s was not found.\n", fullName.c_str());
    return;
  }

  std::shared_ptr<Transform> transformInstance;
  if (formatName == "") {
    transformInstance = tit->second.begin()->second();
  } else {
    auto tfit = tit->second.find(formatName);
    if (tfit == tit->second.end()) {
      fprintf(stderr, "Error: transform %s was not found.\n",
              fullName.c_str());
      return;
    }
    transformInstance = tfit->second();
  }
  copy_string(transformInstance->Description(), description);
  copy_string(transformInstance->InputFormat()->Id(), inputFormat);
  copy_string(transformInstance->OutputFormat()->Id(), outputFormat);
  int pCount = transformInstance->SupportedParameters().size();
  *parametersCount = pCount;
  *parameterNames = new char*[pCount];
  *parameterDescriptions = new char*[pCount];
  *parameterDefaultValues = new char*[pCount];
  int i = 0;
  for (auto prop : transformInstance->SupportedParameters()) {
    copy_string(prop.first, *parameterNames + i);
    copy_string(prop.second.Description, *parameterDescriptions + i);
    copy_string(prop.second.DefaultValue, *parameterDefaultValues + i);
    i++;
  }
}

void destroy_transform_details(char *description,
                               char *inputFormat, char *outputFormat,
                               char **parameterNames,
                               char **parameterDescriptions,
                               char **parameterDefaultValues,
                               int parametersCount) {
  CHECK_NULL(description);
  CHECK_NULL(parameterNames);
  CHECK_NULL(parameterDescriptions);
  CHECK_NULL(parameterDefaultValues);

  delete[] description;
  delete[] inputFormat;
  delete[] outputFormat;
  for (int i = 0; i < parametersCount; i++) {
    delete[] parameterNames[i];
    delete[] parameterDescriptions[i];
    delete[] parameterDefaultValues[i];
  }
  delete[] parameterNames;
  delete[] parameterDescriptions;
  delete[] parameterDefaultValues;
}

FeaturesConfiguration *setup_features_extraction(
    const char *const *features, int featuresCount,
    size_t bufferSize, int samplingRate) {
  CHECK_NULL_RET(features, nullptr);
  if (featuresCount < 0) {
    fprintf(stderr, "Error: featuresCount is negative (%i)\n", featuresCount);
    return nullptr;
  }
  if (featuresCount > MAX_FEATURES_COUNT) {
    fprintf(stderr, "Error: featuresCount is too big "
            "(%i > MAX_FEATURES_COUNT=%i)\n",
            featuresCount, MAX_FEATURES_COUNT);
    return nullptr;
  }
  std::vector<std::string> lines;
  for (int i = 0; i < featuresCount; i++) {
    if (features[i] == nullptr) {
      fprintf(stderr, "Error: features[%i] is null\n", i);
      return nullptr;
    }
    lines.push_back(features[i]);
  }
  RawFeaturesMap featmap;
  try {
    featmap = SoundFeatureExtraction::Features::Parse(lines);
  }
  catch(const ParseFeaturesException& pfe) {
    fprintf(stderr, "Failed to parse features. %s\n", pfe.what());
    return nullptr;
  }

  auto format = std::make_shared<RawFormat16>(bufferSize, samplingRate);
  auto config = new FeaturesConfiguration();
  config->Tree = std::make_unique<TransformTree>(format);
  for (auto featpair : featmap) {
    try {
      config->Tree->AddFeature(featpair.first, featpair.second);
    }
    catch(const ChainNameAlreadyExistsException& cnaee) {
      fprintf(stderr, "Failed to construct the transform tree. %s\n",
              cnaee.what());
      return nullptr;
    }
    catch(const TransformNotRegisteredException& tnre) {
      fprintf(stderr, "Failed to construct the transform tree. %s\n",
              tnre.what());
      return nullptr;
    }
    catch(const ChainAlreadyExistsException& caee) {
      fprintf(stderr, "Failed to construct the transform tree. %s\n",
              caee.what());
      return nullptr;
    }
    catch(const IncompatibleTransformFormatException& itfe) {
      fprintf(stderr, "Failed to construct the transform tree. %s\n",
              itfe.what());
      return nullptr;
    }
  }
  config->Tree->PrepareForExecution();
#ifdef DEBUG
  config->Tree->SetValidateAfterEachTransform(true);
#endif
  return config;
}

FeatureExtractionResult extract_speech_features(
    const FeaturesConfiguration *fc, int16_t *buffer,
    char ***featureNames, void ***results, int **resultLengths) {
  CHECK_NULL_RET(fc, FEATURE_EXTRACTION_RESULT_ERROR);
  CHECK_NULL_RET(buffer, FEATURE_EXTRACTION_RESULT_ERROR);
  CHECK_NULL_RET(results, FEATURE_EXTRACTION_RESULT_ERROR);

  std::unordered_map<std::string, std::shared_ptr<Buffers>> retmap;
  try {
    retmap = fc->Tree->Execute(buffer);
  }
  catch (const std::exception& ex) {
    fprintf(stderr, "Caught an exception with message \"%s\".\n", ex.what());
    return FEATURE_EXTRACTION_RESULT_ERROR;
  }
  *featureNames = new char*[retmap.size()];
  *results = new void*[retmap.size()];
  *resultLengths = new int[retmap.size()];

  int i = 0;
  for (auto res : retmap) {
    copy_string(res.first, *featureNames + i);
    size_t sizeEach = res.second->Format()->SizeInBytes();
    assert(sizeEach > 0);
    size_t size = sizeEach * res.second->Count();
    (*resultLengths)[i] = size;
    (*results)[i] = new char[size];
    for (size_t j = 0; j < res.second->Count(); j++) {
      memcpy(reinterpret_cast<char *>((*results)[i]) + j * sizeEach,
             (*res.second)[j], sizeEach);
    }
    i++;
  }
  return FEATURE_EXTRACTION_RESULT_OK;
}

void report_extraction_time(const FeaturesConfiguration *fc,
                            char ***transformNames,
                            float **values,
                            int *length) {
  CHECK_NULL(fc);
  CHECK_NULL(transformNames);
  CHECK_NULL(values);
  CHECK_NULL(length);

  auto report = fc->Tree->ExecutionTimeReport();
  *length = report.size();
  *transformNames = new char*[*length];
  *values = new float[*length];

  int i = 0;
  for (auto pair : report) {
    copy_string(pair.first, *transformNames + i);
    (*values)[i] = pair.second;
    i++;
  }
}

void destroy_extraction_time_report(char **transformNames,
                                    float *values,
                                    int length) {
  CHECK_NULL(transformNames);
  CHECK_NULL(values);

  delete[] values;
  for (int i = 0; i < length; i++) {
    delete[] transformNames[i];
  }
  delete[] transformNames;
}

void report_extraction_graph(const FeaturesConfiguration *fc,
                             const char *fileName) {
  CHECK_NULL(fc);
  CHECK_NULL(fileName);
  fc->Tree->Dump(fileName);
}

void destroy_features_configuration(FeaturesConfiguration* fc) {
  CHECK_NULL(fc);

  delete fc;
}

void free_results(int featuresCount, char **featureNames,
                  void **results, int *resultLengths) {
  if(featuresCount <= 0) {
    fprintf(stderr, "Warning: free_results() was called with featuresCount"
                    " <= 0, skipped\n");
    return;
  }

  for (int i = 0; i < featuresCount; i++) {
    if (featureNames != nullptr) {
      delete[] featureNames[i];
    }
    if (results != nullptr) {
      delete[] reinterpret_cast<char*>(results[i]);
    }
  }
  if (featureNames != nullptr) {
    delete[] featureNames;
  }
  if (resultLengths != nullptr) {
    delete[] resultLengths;
  }
  if (results != nullptr) {
    delete[] results;
  }
}

void get_set_omp_transforms_max_threads_num(int *value, bool get) {
  static int threads_num = omp_get_max_threads();
  if (get) {
    *value = threads_num;
  } else {
    int max = omp_get_max_threads();
    if (*value > max) {
      fprintf(stderr, "Warning: can not set the maximal number of threads to "
                      "%i (the limit is %i), so set to %i\n",
              *value, max, max);
      threads_num = max;
    } else if (*value < 1) {
      fprintf(stderr, "Warning: can not set the maximal number of threads to "
                      "%i. The value remains the same (%i)\n",
              *value, threads_num);
    } else {
      threads_num = *value;
    }
  }
}

int get_omp_transforms_max_threads_num() {
  int res;
  get_set_omp_transforms_max_threads_num(&res, true);
  return res;
}

void set_omp_transforms_max_threads_num(int value) {
  get_set_omp_transforms_max_threads_num(&value, false);
  fftf_set_openmp_num_threads(get_omp_transforms_max_threads_num());
}

bool get_use_simd() {
  return SimdAware::UseSimd();
}

void set_use_simd(int value) {
  SimdAware::SetUseSimd(value);
}

}
