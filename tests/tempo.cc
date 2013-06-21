/*! @file sbc.cc
 *  @brief Subband based Cepstral Parameters.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include <gtest/gtest.h>
#include "src/transform_tree.h"
#include "src/transform_registry.h"
#include "src/formats/raw_format.h"
#include "tests/speech_sample.inc"

using SoundFeatureExtraction::TransformTree;
using SoundFeatureExtraction::Formats::Raw16;
using SoundFeatureExtraction::BuffersBase;

TEST(Features, Tempo) {
  size_t test_size = 661504;
  TransformTree tt( { test_size, 22050 } );  // NOLINT(*)
  //tt.SetValidateAfterEachTransform(true);
  tt.AddFeature("Tempo", {
      { "Window", "length=512" },
      { "Fork", "factor=6" },
      { "RDFT", "" },
      { "FrequencyBands", "bands=200 400 800 1600 3200" },
      { "IRDFT", "" },
      { "IWindow", "length=512" },
      { "Rectify", "" },
      { "Convolve", "window = half-hanning-right, length=12800" },
      { "Diffrect", "" },
      { "Beat", "" } });
  Raw16 buffers(test_size, 0);
  size_t i;
  for (i = 0; i < test_size; i += sizeof(data) / 2) {
    memcpy(buffers.Data.get() + i, data, sizeof(data) / 2);
  }
  memcpy(buffers.Data.get() + i - sizeof(data) / 2,
         data, test_size - (i - sizeof(data) / 2));
  tt.PrepareForExecution();
  auto res = tt.Execute(buffers);
  ASSERT_EQ(1, res.size());
  res["Tempo"]->Validate();
  tt.Dump("/tmp/tempo.dot");
  auto report = tt.ExecutionTimeReport();
  for (auto r : report) {
    printf("%s:\t%f\n", r.first.c_str(), r.second);
  }
}

#include "tests/google/src/gtest_main.cc"
