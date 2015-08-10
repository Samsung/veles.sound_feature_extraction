/*! @file api.h
 *  @brief Sound Feature Extraction interface.
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

#ifndef INC_SOUND_FEATURE_EXTRACTION_API_H_
#define INC_SOUND_FEATURE_EXTRACTION_API_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__

/* malloc() function attribute */
#define MALLOC __attribute__ ((__malloc__))

#ifndef SOUNDFEATUREEXTRACTION_API_IMPLEMENTATION
/* Mark pointer parameters which must not be NULL */
#define NOTNULL(...) __attribute__ ((__nonnull__ (__VA_ARGS__)))
#else
#define NOTNULL(...)
#endif

/* warn about unused result function attribute */
#define WARN_UNUSED_RESULT __attribute__ ((__warn_unused_result__))

#else

#define MALLOC
#define NOTNULL(...)
#define WARN_UNUSED_RESULT

#endif

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

#define MAX_FEATURES_COUNT 256

typedef enum {
  FEATURE_EXTRACTION_RESULT_OK = 0,
  FEATURE_EXTRACTION_RESULT_ERROR = 1
} FeatureExtractionResult;

typedef struct FeaturesConfiguration FeaturesConfiguration;

/// @brief Allocates and fills the array of transform names.
void query_transforms_list(char ***names, int *listSize) NOTNULL(1, 2);

void destroy_transforms_list(char **names, int listSize) NOTNULL(1);

void query_transform_details(const char *name, char **description,
                             char **inputFormat, char **outputFormat,
                             char ***parameterNames,
                             char ***parameterDescriptions,
                             char ***parameterDefaultValues,
                             int *parametersCount) NOTNULL(1, 2, 3, 4, 5);

void destroy_transform_details(char *description,
                               char *inputFormat, char *outputFormat,
                               char **parameterNames,
                               char **parameterDescriptions,
                               char **parameterDefaultValues,
                               int parametersCount) NOTNULL(1, 2, 3, 4);

void query_format_converters_list(char ***inputFormats, char*** outputFormats,
                                  int *listSize) NOTNULL(1, 2, 3);

void destroy_format_converters_list(char **inputFormats, char** outputFormats,
                                    int listSize) NOTNULL(1);

FeaturesConfiguration *setup_features_extraction(
    const char *const *features, int featuresCount,
    size_t bufferSize, int samplingRate) NOTNULL(1) WARN_UNUSED_RESULT MALLOC;

FeatureExtractionResult extract_sound_features(
    const FeaturesConfiguration *fc, int16_t *buffer,
    char ***featureNames, void ***results, int **resultLengths)
    NOTNULL(1, 2, 3, 4, 5);

void report_extraction_time(const FeaturesConfiguration *fc,
                            char ***transformNames,
                            float **values, int *length) NOTNULL(1, 2, 3, 4);

void destroy_extraction_time_report(char **transformNames,
                                    float *values, int length) NOTNULL(1, 2);

void report_extraction_graph(const FeaturesConfiguration *fc,
                             const char *fileName) NOTNULL(1, 2);

void destroy_features_configuration(FeaturesConfiguration *fc) NOTNULL(1);

void free_results(int featuresCount, char **featureNames,
                  void **results, int *resultLengths);

int get_omp_transforms_max_threads_num(void);

void set_omp_transforms_max_threads_num(int value);

bool get_use_simd(void);

void set_use_simd(int value);

size_t get_cpu_cache_size(void);

void set_cpu_cache_size(size_t value);

size_t get_chunk_size(void);

void set_chunk_size(size_t value);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#ifdef __cplusplus
}
#endif

#endif  // INC_SOUND_FEATURE_EXTRACTION_API_H_
