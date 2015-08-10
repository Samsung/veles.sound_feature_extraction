"""
Created on Mar 21, 2013

@author: Markovtsev Vadim <v.markovtsev@samsung.com>

███████████████████████████████████████████████████████████████████████████████

Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.

███████████████████████████████████████████████████████████████████████████████
"""


import logging
import cffi


class Library(object):
    """
    Loads the SoundFeatureExtraction.so shared library and wraps the handle.
    """

    _lib = None
    _ffi = None
    path = "libSoundFeatureExtraction.so"

    @property
    def lib(self):
        if Library._lib is None:
            Library._lib = self.ffi.dlopen(self.path)
        return Library._lib

    @property
    def ffi(self):
        if Library._ffi is None:
            Library._ffi = cffi.FFI()
            Library._ffi.cdef("""
#define MAX_FEATURES_COUNT 256

typedef enum {
  FEATURE_EXTRACTION_RESULT_OK = 0,
  FEATURE_EXTRACTION_RESULT_ERROR = 1
} FeatureExtractionResult;

typedef struct FeaturesConfiguration FeaturesConfiguration;

/// @brief Allocates and fills the array of transform names.
void query_transforms_list(char ***names, int *listSize);

void destroy_transforms_list(char **names, int listSize);

void query_transform_details(const char *name, char **description,
                             char **inputFormat, char **outputFormat,
                             char ***parameterNames,
                             char ***parameterDescriptions,
                             char ***parameterDefaultValues,
                             int *parametersCount);

void destroy_transform_details(char *description,
                               char *inputFormat, char *outputFormat,
                               char **parameterNames,
                               char **parameterDescriptions,
                               char **parameterDefaultValues,
                               int parametersCount);

void query_format_converters_list(char ***inputFormats, char*** outputFormats,
                                  int *listSize);

void destroy_format_converters_list(char **inputFormats, char** outputFormats,
                                    int listSize);

FeaturesConfiguration *setup_features_extraction(
    const char *const *features, int featuresCount,
    size_t bufferSize, int samplingRate);

FeatureExtractionResult extract_sound_features(
    const FeaturesConfiguration *fc, int16_t *buffer,
    char ***featureNames, void ***results, int **resultLengths);

void report_extraction_time(const FeaturesConfiguration *fc,
                            char ***transformNames,
                            float **values, int *length);

void destroy_extraction_time_report(char **transformNames,
                                    float *values, int length);

void report_extraction_graph(const FeaturesConfiguration *fc,
                             const char *fileName);

void destroy_features_configuration(FeaturesConfiguration *fc);

void free_results(int featuresCount, char **featureNames,
                  void **results, int *resultLengths);

int get_omp_transforms_max_threads_num(void);

void set_omp_transforms_max_threads_num(int value);

bool get_use_simd(void);

void set_use_simd(int value);

size_t get_cpu_cache_size(void);

void set_cpu_cache_size(size_t value);

size_t get_chunk_size(void);

void set_chunk_size(size_t value);""")
        return Library._ffi

    def __getattr__(self, item):
        try:
            return getattr(self.lib, item)
        except AttributeError:
            return getattr(self.ffi, item)
