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

TEST(MFCC, Calculation) {
  TransformTree tt( { 48000, 16000 } );  // NOLINT(*)
  tt.SetValidateAfterEachTransform(true);
  tt.AddChain("Energy", { { "Window", "samples=512" }, { "Energy", "" } });
  tt.AddChain("Centroid", { { "Window", "samples=512" }, { "RDFT", "" },
      { "ComplexMagnitude", "" }, { "Centroid", "" } });
  tt.AddChain("Rolloff", { { "Window", "samples=512" }, { "RDFT", "" },
      { "ComplexMagnitude", "" }, { "Rolloff", "" } });
  tt.AddChain("Flux", { { "Window", "samples=512" }, { "RDFT", "" },
      { "ComplexMagnitude", "" }, { "Flux", "" } });
  tt.AddChain("ZeroCrossings", { { "ZeroCrossings", "" } });
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
