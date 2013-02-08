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

#ifndef API_H_
#define API_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if __GNUC__ >= 4
#pragma GCC visibility push (default)
#endif

#define MAX_FEATURES_COUNT 256

typedef enum {
  FEATURE_EXTRACTION_RESULT_OK,
  FEATURE_EXTRACTION_RESULT_ERROR
} FeatureExtractionResult;

typedef struct FeaturesConfiguration FeaturesConfiguration;

FeaturesConfiguration *setup_features_extraction(
    const char *const *features, int featuresCount,
    size_t bufferSize, int samplingRate);

FeatureExtractionResult extract_speech_features(
    const FeaturesConfiguration *fc, const int16_t *buffer,
    void ***results);

void destroy_features_configuration(FeaturesConfiguration *fc);

void free_results(void **results, int featuresCount);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_API_H_
