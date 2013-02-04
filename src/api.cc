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

extern "C" {

struct FeaturesConfiguration {

};

FeaturesConfiguration *setup_features(const char *const *features,
                                      int featuresCount) {
  return NULL;
}

FeatureExtractionResult extract_speech_features(
    const int16_t *const *buffers, int buffersCount, int sizeEach,
    int samplingRate, const FeaturesConfiguration *fc, void *const *results) {
  return FEATURE_EXTRACTION_RESULT_OK;
}

void destroy_features_configuration(FeaturesConfiguration *fc) {

}

}
