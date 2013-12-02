/*! @file crp.cc
 *  @brief Chroma DCT-Reduced Log Pitch calculation.
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

TEST(Features, CRP) {
  TransformTree tt( { 48000, 22050 } );
  tt.set_validate_after_each_transform(true);
  tt.AddFeature("CRP", { { "Window", "length=4096,step=2048" },{ "RDFT", "" },
      { "SpectralEnergy", "" }, { "FilterBank", "type=midi,number=108,"
          "frequency_min=7.946364,frequency_max=8137.0754,squared=true" },
      { "Log", "add1=true,scale=1000" }, { "DCT", "" },
      { "Selector", "select=70,from=right" }, { "IDCT", "" },
      { "Reorder", "algorithm=chroma" },
      { "Stats", "types=average,interval=10" }
  });
  int16_t* buffers = new int16_t[48000];
  memcpy(buffers, data, sizeof(data));
  tt.PrepareForExecution();
  auto res = tt.Execute(buffers);
  delete[] buffers;
  ASSERT_EQ(1U, res.size());
  res["CRP"]->Validate();
  tt.Dump("/tmp/crp.dot");
  auto report = tt.ExecutionTimeReport();
  for (auto r : report) {
    printf("%s:\t%f\n", r.first.c_str(), r.second);
  }
}

#include "tests/google/src/gtest_main.cc"
