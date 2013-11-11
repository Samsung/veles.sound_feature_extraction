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
#include "src/formats/array_format.h"
#include "tests/speech_sample.inc"

using sound_feature_extraction::TransformTree;
using sound_feature_extraction::BuffersBase;

TEST(Features, MusicalSurface) {
  TransformTree tt( { 48000, 16000 } );  // NOLINT(*)
  tt.set_validate_after_each_transform(true);
  tt.AddFeature("Centroid", {
      { "Window", "length=512,step=205" },
      { "RDFT", "" },
      { "ComplexMagnitude", "" },
      { "Centroid", "" },
      { "Merge", "" },
      { "Stats", "" }
  });
  tt.AddFeature("Rolloff", {
      { "Window", "length=512,step=205" },
      { "RDFT", "" },
      { "ComplexMagnitude", "" },
      { "Rolloff", "" },
      { "Merge", "" },
      { "Stats", "" }
  });
  tt.AddFeature("Flux", {
      { "Window", "length=512,step=205" },
      { "RDFT", "" },
      { "ComplexMagnitude", "" },
      { "Flux", "" },
      { "Merge", "" },
      { "Stats", "" }
  });
  tt.AddFeature("Energy", {
      { "Window", "length=512,step=205" },
      { "Energy", "" },
      { "Merge", "" },
      { "Stats", "" }
  });
  tt.AddFeature("ZeroCrossings", {
      { "Window", "type=rectangular,length=512,step=205" },
      { "ZeroCrossings", "" },
      { "Merge", "" },
      { "Stats", "" }
  });
  int16_t* buffers = new int16_t[48000];
  memcpy(buffers, data, sizeof(data));
  tt.PrepareForExecution();
  auto res = tt.Execute(buffers);
  delete[] buffers;
  ASSERT_EQ(5U, res.size());
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
