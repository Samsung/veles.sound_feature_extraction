/*! @file mfcc.cc
 *  @brief Test which checks the calculation of MFCC.
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

using SoundFeatureExtraction::TransformTree;
using SoundFeatureExtraction::BuffersBase;

TEST(Features, MFCC) {
  TransformTree tt( { 48000, 16000 } );  // NOLINT(*)
  tt.SetValidateAfterEachTransform(true);
  // We have to apply FilterBank twice since Energy results in
  // squared magnitude
  tt.AddFeature("MFCC", { { "Window", "length=512" }, { "RDFT", "" },
      { "SpectralEnergy", "" }, { "FilterBank", "" }, { "FilterBank", "" },
      { "Selector", "length=256" }, { "Log", "" }, { "Square", "" },
      { "DCT", "" } });
  int16_t* buffers = new int16_t[48000];
  memcpy(buffers, data, sizeof(data));
  tt.PrepareForExecution();
  auto res = tt.Execute(buffers);
  delete[] buffers;
  ASSERT_EQ(1U, res.size());
  res["MFCC"]->Validate();
  tt.Dump("/tmp/mfcc.dot");
  auto report = tt.ExecutionTimeReport();
  for (auto r : report) {
    printf("%s:\t%f\n", r.first.c_str(), r.second);
  }
}

TEST(Features, MFCCTrivial) {
  TransformTree tt( { 48000, 16000 } );  // NOLINT(*)
  tt.SetValidateAfterEachTransform(true);
  //tt.SetDumpBuffersAfterEachTransform(true);
  // We have to apply FilterBank twice since Energy results in
  // squared magnitude
  tt.AddFeature("MFCC", { { "Window", "length=512" }, { "RDFT", "" },
      { "SpectralEnergy", "" }, { "FilterBank", "" }, { "FilterBank", "" },
      { "Selector", "length=256" },{ "Log", "" }, { "Square", "" },
      { "DCT", "" } });
  int16_t* buffers = new int16_t[48000];
  for (int i = 0; i < 48000; i++) {
    buffers[i] = 128;
  }
  tt.PrepareForExecution();
  auto res = tt.Execute(buffers);
  delete[] buffers;
  ASSERT_EQ(1U, res.size());
  res["MFCC"]->Validate();
}

#include "tests/google/src/gtest_main.cc"
