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
#include <sound_feature_extraction/api.h>
#undef NOTNULL
#include <cassert>
#include <stddef.h>
#include <fftf/api.h>
#include "src/features_parser.h"
#include "src/make_unique.h"
#include "src/safe_omp.h"
#include "src/simd_aware.h"
#include "src/transform_tree.h"
#include "src/transform_registry.h"

using sound_feature_extraction::Transform;
using sound_feature_extraction::TransformFactory;
using sound_feature_extraction::ChainNameAlreadyExistsException;
using sound_feature_extraction::TransformNotRegisteredException;
using sound_feature_extraction::ChainAlreadyExistsException;
using sound_feature_extraction::IncompatibleTransformFormatException;
using sound_feature_extraction::RawFeaturesMap;
using sound_feature_extraction::features::ParseFeaturesException;
using sound_feature_extraction::TransformTree;
using sound_feature_extraction::formats::ArrayFormat16;
using sound_feature_extraction::BuffersBase;
using sound_feature_extraction::Buffers;
using sound_feature_extraction::SimdAware;

extern "C" {

struct FeaturesConfiguration {
  std::unique_ptr<TransformTree> Tree;
  size_t InputSize;
  int Chunks;
};

/// @brief One second of standard 2-channel 44100Hz audio
size_t chunk_size = 60 * 44100 * 2;

#define BLAME(x) EINA_LOG_ERR("Error: " #x " is null (function %s, " \
                              "line %i)\n", \
                              __FUNCTION__, __LINE__)

#define CHECK_NULL(x) do { if (x == nullptr) { \
    BLAME(x); \
    return; \
} } while (0)

#define CHECK_NULL_RET(x, ret) do { if (x == nullptr) { \
    BLAME(x); \
    return ret; \
} } while (0)

static void copy_string(const std::string& str, char **ptr) {
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
    EINA_LOG_ERR("Error: transform %s was not found.\n", fullName.c_str());
    return;
  }

  std::shared_ptr<Transform> transformInstance;
  if (formatName == "") {
    transformInstance = tit->second.begin()->second();
  } else {
    auto tfit = tit->second.find(formatName);
    if (tfit == tit->second.end()) {
      EINA_LOG_ERR("Error: transform %s was not found.\n",
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
  EINA_LOG_DBG("featuresCount=%d, bufferSize=%zu, samplingRate=%i",
      featuresCount, bufferSize, samplingRate);
  if (featuresCount < 0) {
    EINA_LOG_ERR("Error: featuresCount is negative (%i)\n", featuresCount);
    return nullptr;
  }
  if (featuresCount > MAX_FEATURES_COUNT) {
    EINA_LOG_ERR("Error: featuresCount is too big "
              "(%i > MAX_FEATURES_COUNT=%i)\n",
              featuresCount, MAX_FEATURES_COUNT);
    return nullptr;
  }
  std::vector<std::string> lines;
  for (int i = 0; i < featuresCount; i++) {
    if (features[i] == nullptr) {
      EINA_LOG_ERR("Error: features[%i] is null\n", i);
      return nullptr;
    }
    lines.push_back(features[i]);
  }
  RawFeaturesMap featmap;
  try {
    featmap = sound_feature_extraction::features::Parse(lines);
  }
  catch(const ParseFeaturesException& pfe) {
    EINA_LOG_ERR("Failed to parse features. %s\n", pfe.what());
    return nullptr;
  }

  int chunks = 1;
  while (bufferSize / chunks > chunk_size) {
    chunks++;
  }
  auto format = std::make_shared<ArrayFormat16>(
      std::min(bufferSize, bufferSize / chunks), samplingRate);
  auto config = new FeaturesConfiguration();
  config->Tree = std::make_unique<TransformTree>(format);
  config->InputSize = bufferSize;
  config->Chunks = chunks;
  for (auto& featpair : featmap) {
    try {
      config->Tree->AddFeature(featpair.first, featpair.second);
    }
    catch(const ChainNameAlreadyExistsException& cnaee) {
      EINA_LOG_ERR("Failed to construct the transform tree. %s\n",
              cnaee.what());
      return nullptr;
    }
    catch(const TransformNotRegisteredException& tnre) {
      EINA_LOG_ERR("Failed to construct the transform tree. %s\n",
              tnre.what());
      return nullptr;
    }
    catch(const ChainAlreadyExistsException& caee) {
      EINA_LOG_ERR("Failed to construct the transform tree. %s\n",
              caee.what());
      return nullptr;
    }
    catch(const IncompatibleTransformFormatException& itfe) {
      EINA_LOG_ERR("Failed to construct the transform tree. %s\n",
              itfe.what());
      return nullptr;
    }
  }
  config->Tree->PrepareForExecution();
#ifdef DEBUG
  config->Tree->set_validate_after_each_transform(true);
#endif
  return config;
}

FeatureExtractionResult extract_speech_features(
    const FeaturesConfiguration *fc, int16_t *buffer,
    char ***featureNames, void ***results, int **resultLengths) {
  CHECK_NULL_RET(fc, FEATURE_EXTRACTION_RESULT_ERROR);
  CHECK_NULL_RET(buffer, FEATURE_EXTRACTION_RESULT_ERROR);
  CHECK_NULL_RET(results, FEATURE_EXTRACTION_RESULT_ERROR);

  fftf_set_openmp_num_threads(get_omp_transforms_max_threads_num());
  EINA_LOG_DBG("OpenMP threads number is %d, SIMD is %s, FFTF backend is %d\n",
               get_omp_transforms_max_threads_num(),
               get_use_simd()? "enabled" : "disabled",
               fftf_current_backend());
  std::unordered_map<std::string, std::shared_ptr<Buffers>> retmap;
  size_t step = fc->InputSize / fc->Chunks;
  size_t length = step * fc->Chunks;
  for (size_t i = 0; i < length; i += step) {
    EINA_LOG_INFO("Evaluating [%d%%, %d%%]...",
                  static_cast<int>(i * 100 / length),
                  static_cast<int>((i + step) * 100 / length));
    try {
      retmap = fc->Tree->Execute(buffer + i);
    }
    catch(const std::exception& ex) {
      EINA_LOG_ERR("Caught an exception with message \"%s\".\n", ex.what());
      return FEATURE_EXTRACTION_RESULT_ERROR;
    }
    if (i == 0) {
      *featureNames = new char*[retmap.size()];
      *results = new void*[retmap.size()];
      *resultLengths = new int[retmap.size()];
    }

    int j = 0;
    for (auto& res : retmap) {
      if (i == 0) {
        copy_string(res.first, *featureNames + j);
      }
      size_t size_each = res.second->Format()->UnalignedSizeInBytes();
      assert(size_each > 0);
      size_t size = size_each * res.second->Count();
      if (i == 0) {
        (*resultLengths)[j] = 0;
        (*results)[j] = new char[size * fc->Chunks];
      }
      for (size_t k = 0; k < res.second->Count(); k++) {
        memcpy(reinterpret_cast<char *>((*results)[j]) +
                   (*resultLengths)[j] + k * size_each,
               (*res.second)[k], size_each);
      }
      (*resultLengths)[j] += size;
      j++;
    }
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
  if (featuresCount <= 0) {
    EINA_LOG_ERR("Warning: free_results() was called with featuresCount"
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
      EINA_LOG_ERR("Warning: can not set the maximal number of threads to "
                   "%i (the limit is %i), so set to %i\n",
                   *value, max, max);
      threads_num = max;
    } else if (*value < 1) {
      EINA_LOG_ERR("Warning: can not set the maximal number of threads to "
                   "%i. The value remains the same (%i)\n",
                   *value, threads_num);
    } else {
      threads_num = *value;
    }
  }
}

int get_omp_transforms_max_threads_num(void) {
  int res;
  get_set_omp_transforms_max_threads_num(&res, true);
  return res;
}

void set_omp_transforms_max_threads_num(int value) {
  get_set_omp_transforms_max_threads_num(&value, false);
  fftf_set_openmp_num_threads(get_omp_transforms_max_threads_num());
}

bool get_use_simd(void) {
  return SimdAware::use_simd();
}

void set_use_simd(int value) {
  SimdAware::set_use_simd(value);
}

size_t cpu_cache_size = 8 * 1024 * 1024;

size_t get_cpu_cache_size() {
  return cpu_cache_size;
}

void set_cpu_cache_size(size_t value) {
  if (value > 0) {
    cpu_cache_size = value;
  } else {
    EINA_LOG_ERR("CPU cache size must be greater than zero.");
  }
}

size_t get_chunk_size(void) {
  return chunk_size;
}

void set_chunk_size(size_t value) {
  if (value > 0) {
    chunk_size = value;
  } else {
    EINA_LOG_ERR("Cache size must be greater than zero.");
  }
}

}  // extern "C"
