/*! @file musical_surface.cc
 *  @brief Musical surface features.
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

TEST(Features, MusicalSurface) {
  TransformTree tt( { 48000, 16000 } );  // NOLINT(*)
  tt.SetValidateAfterEachTransform(true);
  tt.AddFeature("Centroid", {
      { "Window", "length=512,step=205" },
      { "RDFT", "" },
      { "ComplexMagnitude", "" },
      { "Centroid", "" },
      { "Stats", "" }
  });
  tt.AddFeature("Rolloff", {
      { "Window", "length=512,step=205" },
      { "RDFT", "" },
      { "ComplexMagnitude", "" },
      { "Rolloff", "" },
      { "Stats", "" }
  });
  tt.AddFeature("Flux", {
      { "Window", "length=512,step=205" },
      { "RDFT", "" },
      { "ComplexMagnitude", "" },
      { "Flux", "" },
      { "Stats", "" }
  });
  tt.AddFeature("Energy", {
      { "Window", "length=512,step=205" },
      { "Energy", "" },
      { "Stats", "" }
  });
  tt.AddFeature("ZeroCrossings", {
      { "Window", "type=rectangular,length=512,step=205" },
      { "ZeroCrossings", "" },
      { "Stats", "" }
  });
  Raw16 buffers(48000, 0);
  memcpy(buffers.Data.get(), data, sizeof(data));
  tt.PrepareForExecution();
  auto res = tt.Execute(buffers);
  ASSERT_EQ(5, res.size());
  res["Energy"]->Validate();
  res["Centroid"]->Validate();
  res["Rolloff"]->Validate();
  res["Flux"]->Validate();
  res["ZeroCrossings"]->Validate();
  tt.Dump("/tmp/msurf.dot");
  auto report = tt.ExecutionTimeReport();
  for (auto r : report) {
    printf("%s:\t%f\n", r.first.c_str(), r.second);
  }
}

#include "tests/google/src/gtest_main.cc"
