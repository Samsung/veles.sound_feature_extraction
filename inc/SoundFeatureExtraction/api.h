/*! @file api.h
 *  @brief Speech Feature Extraction interface.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef INC_SPFEXTR_API_H_
#define INC_SPFEXTR_API_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__

/* malloc() function attribute */
#define MALLOC __attribute__ ((__malloc__))

/* Mark pointer parameters which must not be NULL */
#define NOTNULL(...) __attribute__ ((__nonnull__ (__VA_ARGS__)))

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

FeatureExtractionResult extract_speech_features(
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
                  void **results, int *resultLengths) NOTNULL(2, 3, 4);

int get_omp_transforms_max_threads_num();

void set_omp_transforms_max_threads_num(int value);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#ifdef __cplusplus
}
#endif

#endif  // INC_SPFEXTR_API_H_
