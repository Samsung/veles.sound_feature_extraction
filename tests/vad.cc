/*! @file vad.cc
 *  @brief Voice Activity Detection features.
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
using SoundFeatureExtraction::BuffersBase;

TEST(Features, VAD) {
  TransformTree tt( { 48000, 16000 } );  // NOLINT(*)
  tt.SetValidateAfterEachTransform(true);
  tt.AddFeature("Energy", { { "Window", "length=512" }, { "Energy", "" } });
  tt.AddFeature("SFM", { { "Window", "length=512" }, { "RDFT", "" },
      { "ComplexMagnitude", "" }, { "Mean", "types=arithmetic geometric" },
      { "SFM", "" } });
  tt.AddFeature("DominantFrequency", { { "Window", "length=512" }, { "RDFT", "" },
      { "ComplexMagnitude", "" }, { "ArgMinMax", "extremum=max" } });
  int16_t* buffers = new int16_t[48000];
  memcpy(buffers, data, sizeof(data));
  tt.PrepareForExecution();
  auto res = tt.Execute(buffers);
  ASSERT_EQ(3, res.size());
  res["SFM"]->Validate();
  res["Energy"]->Validate();
  res["DominantFrequency"]->Validate();
  tt.Dump("/tmp/vad.dot");
  auto report = tt.ExecutionTimeReport();
  for (auto r : report) {
    printf("%s:\t%f\n", r.first.c_str(), r.second);
  }
}

#include "tests/google/src/gtest_main.cc"
