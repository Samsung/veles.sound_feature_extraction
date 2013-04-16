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
using SoundFeatureExtraction::Formats::Raw16;
using SoundFeatureExtraction::BuffersBase;

TEST(MFCC, Calculation) {
  ASSERT_NO_THROW({
    TransformTree tt( { 48000, 16000 } );  // NOLINT(*)
    tt.SetValidateAfterEachTransform(true);
    tt.AddChain("Energy", { { "Window", "length=32" }, { "Energy", "" } });
    tt.AddChain("SFM", { { "Window", "length=32" }, { "RDFT", "" },
        { "ComplexMagnitude", "" }, { "Mean", "types=arithmetic geometric" },
        { "SFM", "" } });
    tt.AddChain("DominantFrequency", { { "Window", "length=32" }, { "RDFT", "" },
        { "ComplexMagnitude", "" }, { "ArgMinMax", "extremum=max" } });
    BuffersBase<Raw16> buffers(tt.RootFormat());
    buffers.Initialize(1, 48000, 0);
    memcpy(buffers[0]->Data.get(), data, sizeof(data));
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
  });
}

#include "tests/google/src/gtest_main.cc"
